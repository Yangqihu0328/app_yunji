#include "MqttClient.hpp"

#include "ax_base_type.h"
#include "cmdline.hpp"
#include "AppLog.hpp"
#include "DiskHelper.hpp"
#include "ElapsedTimer.hpp"
#include "BoxBuilder.hpp"
#include "EncoderOptionHelper.h"
#include "BoxMediaParser.hpp"
#include "BoxModelParser.hpp"

#include "httplib.h"
#include "nlohmann/json.hpp"

#include <linux/rtnetlink.h>

namespace boxconf {

#define MQTT_CLIENT "MQTT_CLIENT"

#define ZLM_IP      "127.0.0.1"
#define ZLM_API_URL "http://" ZLM_IP ":80"
#define ZLM_SECRET  "81DBE7AF-ACD5-47D8-A692-F4B27456E6FD"

#define ALARM_IMG_PATH "ZLMediaKit/www/alarm"

using namespace std;
using json = nlohmann::json;

//需要转为类成员变量，提高代码简洁性
static std::shared_ptr<IPStack> ipstack_ = nullptr;
static std::shared_ptr<MQTT::Client<IPStack, Countdown>> client_ = nullptr;
static int arrivedcount = 0;
static std::queue<StreamCmd> StreamQueue;
static std::mutex mtx;

static string GetExecPath(AX_VOID) {
    string strPath;
    AX_CHAR szPath[260] = {0};
    ssize_t sz = readlink("/proc/self/exe", szPath, sizeof(szPath));
    if (sz <= 0) {
        strPath = "./";
    } else {
        strPath = szPath;
        strPath = strPath.substr(0, strPath.rfind('/') + 1);
    }

    return strPath;
}

static int GetVersion(std::string& version) {
    std::ifstream temp_file("/proc/ax_proc/version");
    if (!temp_file) {
        std::cerr << "Cannot open temperature file" << std::endl;
        return -1;
    }

    std::string temp_line;
    if (!std::getline(temp_file, temp_line)) {
        std::cerr << "Cannot read temperature" << std::endl;
        return -1;
    }

    version = std::move(temp_line);
    return 0;
}

static int GetTemperature(int &temp) {
    std::ifstream temp_file("/sys/class/thermal/thermal_zone0/temp");
    if (!temp_file) {
        std::cerr << "Cannot open temperature file" << std::endl;
        temp = -1;
        return -1;
    }

    std::string temp_line;
    if (!std::getline(temp_file, temp_line)) {
        std::cerr << "Cannot read temperature" << std::endl;
        temp = -1;
        return -1;
    }

    long tempMicroCelsius = std::stol(temp_line);
    temp = static_cast<int>(tempMicroCelsius / 1000.0);
    return 0;
}

static int GetSystime(std::string& timeString) {
    auto now = std::chrono::system_clock::now();
    std::time_t currentTime = std::chrono::system_clock::to_time_t(now);
    std::tm* localTime = std::localtime(&currentTime);

    std::ostringstream oss;
    oss << std::put_time(localTime, "%Y-%m-%d %H:%M:%S");
    timeString = oss.str();
    return 0;
}

int GetIP(AX_CHAR* pOutIPAddress) {
    std::string strDevPrefix = "eth";
    for (char c = '0'; c <= '9'; ++c) {
        std::string strDevice = strDevPrefix + c;
        int fd;
        struct ifreq ifr;
        fd = socket(AF_INET, SOCK_DGRAM, 0);
        strcpy(ifr.ifr_name, strDevice.c_str());
        if (ioctl(fd, SIOCGIFADDR, &ifr) < 0) {
            ::close(fd);
            continue;
        }

        char* pIP = inet_ntoa(((struct sockaddr_in*)&(ifr.ifr_addr))->sin_addr);
        if (pIP) {
            strcpy((char*)pOutIPAddress, pIP);
            ::close(fd);
            return 0;
        }
    }

    return -1;
}

static int GetNPUInfo(std::vector<int> &npu_utilization) {
    static int read_flag = 0;
    if (read_flag == 0) {
        std::ofstream enable_file("/proc/ax_proc/npu/enable");
        if (!enable_file) {
            LOG_M_E(MQTT_CLIENT, "Cannot open the file.");
            return -1;
        }

        enable_file << "1";
        enable_file.close();
        read_flag = 1;
    }

    std::ifstream temp_file("/proc/ax_proc/npu/top");
    if (!temp_file) {
        LOG_M_E(MQTT_CLIENT, "Cannot open temperature file.");
        return -1;
    }

    std::string line;

    // 读取文件内容
    while (std::getline(temp_file, line)) {
        if (line.find("core:vnpu_stdd_1") != std::string::npos) {
            std::getline(temp_file, line); // 读取时间
            std::getline(temp_file, line); // 读取周期
            std::getline(temp_file, line); // 读取利用率
            npu_utilization[0] = std::stoi(line.substr(line.find(':') + 1, line.find('%') - line.find(':') - 1));
        } else if (line.find("core:vnpu_stdd_2") != std::string::npos) {
            std::getline(temp_file, line);
            std::getline(temp_file, line);
            std::getline(temp_file, line);
            npu_utilization[1] = std::stoi(line.substr(line.find(':') + 1, line.find('%') - line.find(':') - 1));
        } else if (line.find("core:vnpu_stdd_3") != std::string::npos) {
            std::getline(temp_file, line);
            std::getline(temp_file, line);
            std::getline(temp_file, line);
            npu_utilization[2] = std::stoi(line.substr(line.find(':') + 1, line.find('%') - line.find(':') - 1));
        }
    }

    temp_file.close();
    return 0;
}

static int GetMemoryInfo(MemoryInfo &memInfo) {
    std::ifstream meminfoFile("/proc/meminfo");
    if (!meminfoFile.is_open()) {
        std::cerr << "Unable to open /proc/meminfo" << std::endl;
        return -1;
    }

    std::string line;
    while (std::getline(meminfoFile, line)) {
        std::istringstream iss(line);
        std::string key;
        long value;
        std::string unit;
        iss >> key >> value >> unit;

        if (key == "MemTotal:") {
            memInfo.totalMem = value;
        } else if (key == "MemFree:") {
            memInfo.freeMem = value;
        } else if (key == "MemAvailable:") {
            memInfo.availableMem = value;
        } else if (key == "Buffers:") {
            memInfo.buffers = value;
        } else if (key == "Cached:") {
            memInfo.cached = value;
        }
        memInfo.usedMem = memInfo.totalMem - memInfo.availableMem;
    }
    meminfoFile.close();
    return 0;
}

static int GetDiskUsage(const std::string& path, FlashInfo &falsh_info) {
    struct statvfs stat;

    if (statvfs(path.c_str(), &stat) != 0) {
        // 错误处理
        perror("statvfs");
        return -1;
    }

    falsh_info.total = stat.f_blocks * stat.f_frsize;
    falsh_info.free = stat.f_bfree * stat.f_frsize;
    falsh_info.used = falsh_info.total - falsh_info.free;
    return 0;
}

static bool SendMsg(const char *topic, const char *msg, size_t len) {
    LOG_M_C(MQTT_CLIENT, "SendMsg ++++.");

    MQTT::Message message;
    message.qos = MQTT::QOS0;
    message.retained = false;
    message.dup = false;
    message.id = 0;
    message.payload = (void *)msg;
    message.payloadlen = len;
    if (client_) {
        int ret = client_->publish(topic, message);
        if (ret != 0) {
            LOG_M_E(MQTT_CLIENT, "publish message len %ld failed %d...\n", len, ret);
        }
    } else {
        LOG_M_E(MQTT_CLIENT, "MQTT CLIENT can not get client to publish");
    }

    LOG_M_C(MQTT_CLIENT, "SendMsg ----.");

    return true;
}

static void OnLogin(const std::string& account, const std::string& password) {
    LOG_M_C(MQTT_CLIENT, "OnLogin ++++.");

    json child;
    child["type"] = "login";

    json root;
    root["data"] = child;
    if (account == "admin" && password == "admin") {
        root["result"] = 0;
        root["msg"] = "success";
    } else {
        root["result"] = -1;
        root["msg"] = "The account or password is incorrect";
    }

    std::string payload = root.dump();
    SendMsg("web-message", payload.c_str(), payload.size());

    LOG_M_C(MQTT_CLIENT, "OnLogin ----.");
}

static void OnGetDashBoardInfo() {
    LOG_M_C(MQTT_CLIENT, "OnGetDashBoardInfo ++++.");

    int temperature = -1;
    int ret = GetTemperature(temperature);
    if (ret == -1) {
        LOG_MM_D(MQTT_CLIENT, "GetTemperature fail.");
    }

    AX_CHAR szIP[64] = {0};
    ret = GetIP(szIP);
    if (ret == -1) {
        LOG_MM_D(MQTT_CLIENT, "GetIP fail.");
    }

    MemoryInfo memInfo = {0};
    ret = GetMemoryInfo(memInfo);
    if (ret == -1) {
        LOG_MM_D(MQTT_CLIENT, "MqttMemoryInfo fail.");
    }

    FlashInfo falsh_info = {0};
    ret = GetDiskUsage("/", falsh_info);

    std::string currentTimeStr;
    GetSystime(currentTimeStr);

    std::string version;
    GetVersion(version);

    std::vector<int> npu_utilization(3, 0); // 初始化三个 NPU 的利用率为 0
    GetNPUInfo(npu_utilization);

    json board_info = {
        {"type", "getDashBoardInfo"}, 
        {"BoardId", "YJ-AIBOX-001"}, 
        {"BoardIp", szIP},
        {"BoardPlatform", "AX650"},
        {"BoardTemp", temperature},
        {"BoardType", "LAN"},
        {"BoardAuthorized", "Authorized"},
        {"Time", currentTimeStr},
        {"Version", version},
        {"HostDisk", { // 当前设备硬盘情况 kB
            {"Available", falsh_info.free}, // 可用
            {"Total", falsh_info.total}, // 总量
            {"Used", falsh_info.used}, // 已量
        }},
        {"HostMemory", { // 当前设备内存使用情况
            {"Available", memInfo.availableMem}, // 可用
            {"Total", memInfo.totalMem}, // 总量
            {"Used", memInfo.usedMem}, // 已用
        }},
        {"Tpu", { // 当前设备的算力资源使用情况
            {
                {"vpu0_usage", npu_utilization[0]},
                {"vpu1_usage", npu_utilization[1]},
                {"vpu2_usage", npu_utilization[2]}
            }
        }},
    };

    json root;
    root["result"] = 0;
    root["msg"] = "success";
    root["data"] = board_info;

    std::string payload = root.dump();
    SendMsg("web-message", payload.c_str(), payload.size());

    LOG_M_C(MQTT_CLIENT, "OnGetDashBoardInfo ----.");
}

static void OnRebootAiBox() {
    LOG_M_D(MQTT_CLIENT, "OnRebootAiBox ++++.");

    json child;
    child["type"] = "rebootAiBox";

    json root;
    root["result"] = 0;
    root["msg"] = "success";
    root["data"] = child;

    std::string payload = root.dump();

    SendMsg("web-message", payload.c_str(), payload.size());

    system("reboot");

    LOG_M_D(MQTT_CLIENT, "OnRebootAiBox ----.");
}

static void OnRestartAppService() {
    LOG_M_C(MQTT_CLIENT, "OnRestartAppService ++++.");

    json child;
    child["type"] = "restartAppService";

    json root;
    root["result"] = 0;
    root["msg"] = "success";
    root["data"] = child;

    std::string payload = root.dump();

    SendMsg("web-message", payload.c_str(), payload.size());

    system("/usr/bin/systemctl restart yj-aibox");

    LOG_M_C(MQTT_CLIENT, "OnRestartAppService ----.");
}

static void OnSyncSystemTime(int year, int month, int day, int hour, int minute, int second) {
    LOG_M_C(MQTT_CLIENT, "OnSyncSystemTime ++++.");

    json child;
    child["type"] = "syncSystemTime";

    json root;

    // sync system time
    struct tm timeinfo = {};
    timeinfo.tm_year = year - 1900; // tm_year 是从 1900 年开始的
    timeinfo.tm_mon = month - 1;    // tm_mon 是从 0 开始的
    timeinfo.tm_mday = day;
    timeinfo.tm_hour = hour;
    timeinfo.tm_min = minute;
    timeinfo.tm_sec = second;

    time_t newtime = mktime(&timeinfo);
    if (newtime == -1) {
        LOG_M_E(MQTT_CLIENT, "Failed to convert tm to time_t.");

        root["result"] = -1;
        root["msg"] = "failed";
        root["data"] = child;

        std::string payload = root.dump();
        SendMsg("web-message", payload.c_str(), payload.size());

        return;
    }

    struct timeval tv;
    tv.tv_sec = newtime;
    tv.tv_usec = 0;

    if (settimeofday(&tv, nullptr) == -1) {
        root["result"] = -1;
        root["msg"] = "failed";
    } else {
        root["result"] = 0;
        root["msg"] = "success";
    }
    root["data"] = child;

    std::string payload = root.dump();
    SendMsg("web-message", payload.c_str(), payload.size());

    LOG_M_C(MQTT_CLIENT, "OnSyncSystemTime ----.");
}


// 这个函数可能会有耗时问题
bool check_RTSP_stream(const std::string& rtspUrl) {
    LOG_M_C(MQTT_CLIENT, "check_RTSP_stream ++++.");

    AVFormatContext* formatContext = avformat_alloc_context();
    if (!formatContext) {
        LOG_M_D(MQTT_CLIENT, "avformat_alloc_context(stream %d) failed!");
        return false;
    }

    if (avformat_open_input(&formatContext, rtspUrl.c_str(), nullptr, nullptr) < 0) {
        LOG_M_D(MQTT_CLIENT, "Failed to open RTSP stream: ", rtspUrl);
        return false;
    }

    if (avformat_find_stream_info(formatContext, nullptr) < 0) {
        LOG_M_D(MQTT_CLIENT, "Failed to retrieve stream info.");
        avformat_close_input(&formatContext);
        return false;
    }

    avformat_close_input(&formatContext);

    LOG_M_C(MQTT_CLIENT, "check_RTSP_stream ++++.");

    return true;
}

static void OnGetMediaChannelList() {
    LOG_M_C(MQTT_CLIENT, "OnGetMediaChannelList ++++.");

    // 获取当前通道信息
    AX_U32 nMediaCnt = 0;
    STREAM_CONFIG_T streamConfig = CBoxConfig::GetInstance()->GetStreamConfig();
    std::vector<MEDIA_INFO_T> mediasMap = CBoxMediaParser::GetInstance()->GetMediasMap(&nMediaCnt, streamConfig.strMediaPath);

    json arr = nlohmann::json::array();
    for (size_t i = 0; i < mediasMap.size(); i++) {
        arr.push_back({
            {"mediaId",     mediasMap[i].nMediaId},
            {"mediaDelete", mediasMap[i].nMediaDelete},
            {"mediaStatus", mediasMap[i].nMediaStatus},
            {"mediaUrl",    mediasMap[i].szMediaUrl},
            {"mediaName",   mediasMap[i].szMediaName},
            {"mediaDesc",   mediasMap[i].szMediaDesc}
        });
    }

    json child;
    child["type"] = "getMediaChannelList";
    child["channels"] = arr;

    json root;
    root["result"] = 0;
    root["msg"] = "success";
    root["data"] = child;

    std::string payload = root.dump();
    SendMsg("web-message", payload.c_str(), payload.size());

    LOG_M_C(MQTT_CLIENT, "OnGetMediaChannelList ----.");
}

static void OnSetMediaChannelInfo(AX_U32 id, const std::string& mediaUrl, const std::string& mediaName, const std::string& mediaDesc) {
    LOG_M_C(MQTT_CLIENT, "OnSetMediaChannelInfo ++++.");

    json root;
    // 获取当前通道信息
    AX_U32 nMediaCnt = 0;
    STREAM_CONFIG_T streamConfig = CBoxConfig::GetInstance()->GetStreamConfig();
    std::vector<MEDIA_INFO_T> mediasMap = CBoxMediaParser::GetInstance()->GetMediasMap(&nMediaCnt, streamConfig.strMediaPath);
    if (id < (AX_U32)mediasMap.size()) {
        mediasMap[id].nMediaId = id;
        mediasMap[id].nMediaDelete = 0;
        mediasMap[id].nMediaStatus = 1; // 0异常 1正常/未使用 2使用中
        strcpy(mediasMap[id].szMediaUrl, mediaUrl.c_str());
        strcpy(mediasMap[id].szMediaName, mediaName.c_str());
        strcpy(mediasMap[id].szMediaDesc, mediaDesc.c_str());

        // 更新配置
        CBoxMediaParser::GetInstance()->SetMediasMap(mediasMap);

        root["result"] = 0;
        root["msg"] = "success";
    } else {
        root["result"] = -1;
        root["msg"] = "invalid stream id!";
    }

    json child;
    child["type"] = "setMediaChannelInfo";
    child["status"] = 0; // 0异常 1正常

    root["data"] = child;

    std::string payload = root.dump();
    SendMsg("web-message", payload.c_str(), payload.size());

    LOG_M_C(MQTT_CLIENT, "OnSetMediaChannelInfo ----.");
}

static void OnDelMediaChannelInfo(AX_U32 id) {
    LOG_M_C(MQTT_CLIENT, "OnDelMediaChannelInfo ++++.");

    json root;

    // 获取当前通道信息
    AX_U32 nMediaCnt = 0;
    STREAM_CONFIG_T streamConfig = CBoxConfig::GetInstance()->GetStreamConfig();
    std::vector<MEDIA_INFO_T> mediasMap = CBoxMediaParser::GetInstance()->GetMediasMap(&nMediaCnt, streamConfig.strMediaPath);
    if (id < (AX_U32)mediasMap.size()) {
        mediasMap[id].nMediaDelete = 1;

        // 更新配置
        CBoxMediaParser::GetInstance()->SetMediasMap(mediasMap);

        root["result"] = 0;
        root["msg"] = "success";
    } else {
        root["result"] = -1;
        root["msg"] = "invalid stream id!";
    }

    json child;
    child["type"] = "delMediaChannelInfo";

    root["data"] = child;

    std::string payload = root.dump();
    SendMsg("web-message", payload.c_str(), payload.size());

    LOG_M_C(MQTT_CLIENT, "OnDelMediaChannelInfo ----.");
}

static void OnGetAiModelList() {
    LOG_M_C(MQTT_CLIENT, "OnGetAiModelList ++++.");

    // 获取当前通道信息
    AX_U32 nModelCnt = 0;
    STREAM_CONFIG_T streamConfig = CBoxConfig::GetInstance()->GetStreamConfig();
    std::vector<MODEL_INFO_T> modelsMap = CBoxModelParser::GetInstance()->GetModelsMap(&nModelCnt, streamConfig.strModelPath);

    json arr = nlohmann::json::array();
    for (size_t i = 0; i < modelsMap.size(); i++) {
        arr.push_back({
            {"modelId",      modelsMap[i].nModelId},
            {"modelPath",    modelsMap[i].szModelPath},
            {"modelName",    modelsMap[i].szModelName},
            {"modelDesc",    modelsMap[i].szModelDesc},
            {"modelVersion", modelsMap[i].szModelVersion}
        });
    }

    json child;
    child["type"] = "getAiModelList";
    child["models"] = arr;

    json root;
    root["msg"] = "success";
    root["data"] = child;

    std::string payload = root.dump();
    SendMsg("web-message", payload.c_str(), payload.size());

    LOG_M_C(MQTT_CLIENT, "OnGetAiModelList ----.");
}

static void OnGetAlgoTaskList() {
    LOG_M_C(MQTT_CLIENT, "OnGetAlgoTaskList ++++.");

    // 获取当前通道信息
    AX_U32 nMediaCnt = 0;
    STREAM_CONFIG_T streamConfig = CBoxConfig::GetInstance()->GetStreamConfig();
    std::vector<MEDIA_INFO_T> mediasMap = CBoxMediaParser::GetInstance()->GetMediasMap(&nMediaCnt, streamConfig.strMediaPath);

    json arr = nlohmann::json::array();
    for (size_t i = 0; i < mediasMap.size(); i++) {
        json leaf;
        leaf["mediaId"]     = mediasMap[i].nMediaId;
        leaf["mediaUrl"]    = mediasMap[i].szMediaUrl;
        leaf["taskDelete"]  = mediasMap[i].taskInfo.nTaskDelete;
        leaf["taskStatus"]  = mediasMap[i].taskInfo.nTaskStatus;
        leaf["taskPushUrl"] = mediasMap[i].taskInfo.szPushUrl;
        leaf["taskName"]    = mediasMap[i].taskInfo.szTaskName;
        leaf["taskDesc"]    = mediasMap[i].taskInfo.szTaskDesc;
        leaf["taskKey"]     = mediasMap[i].taskInfo.szTaskKey;
        json algos = nlohmann::json::array();
        for (size_t j = 0; j < mediasMap[i].taskInfo.vAlgo.size(); j++) {
            algos.push_back(mediasMap[i].taskInfo.vAlgo[j]);
        }
        leaf["taskAlgos"]       = algos;

        arr.push_back(leaf);
    }

    json child;
    child["type"] = "getAlgoTaskList";
    child["tasks"] = arr;

    json root;
    root["msg"] = "success";
    root["data"] = child;

    std::string payload = root.dump();
    SendMsg("web-message", payload.c_str(), payload.size());

    LOG_M_C(MQTT_CLIENT, "OnGetAlgoTaskList ----.");
}

static void OnSetAlgoTaskInfo(AX_U32 id, const std::string& pushUrl, const std::string& taskName, const std::string& taskDesc, std::vector<AX_U32> vAlgo) {
    LOG_M_C(MQTT_CLIENT, "OnSetAlgoTaskInfo ++++.");

    json root;

    // 获取当前通道信息
    AX_U32 nMediaCnt = 0;
    STREAM_CONFIG_T streamConfig = CBoxConfig::GetInstance()->GetStreamConfig();
    std::vector<MEDIA_INFO_T> mediasMap = CBoxMediaParser::GetInstance()->GetMediasMap(&nMediaCnt, streamConfig.strMediaPath);
    if (id < (AX_U32)mediasMap.size()) {
        mediasMap[id].nMediaStatus = 2; // 0异常 1正常/未使用 2使用中
        mediasMap[id].taskInfo.nTaskDelete = 0;
        mediasMap[id].taskInfo.nTaskStatus = 0; // 0未运行 1运行中
        strcpy(mediasMap[id].taskInfo.szPushUrl, pushUrl.c_str());
        strcpy(mediasMap[id].taskInfo.szTaskName, taskName.c_str());
        strcpy(mediasMap[id].taskInfo.szTaskDesc, taskDesc.c_str());
        mediasMap[id].taskInfo.vAlgo = vAlgo;

        // 更新配置
        CBoxMediaParser::GetInstance()->SetMediasMap(mediasMap);

        root["result"] = 0;
        root["msg"] = "success";
    } else {
        root["result"] = -1;
        root["msg"] = "invalid stream id!";
    }

    json child;
    child["type"] = "setAlgoTaskInfo";
    child["status"] = 0; // 0异常 1正常

    root["data"] = child;

    std::string payload = root.dump();
    SendMsg("web-message", payload.c_str(), payload.size());

    LOG_M_C(MQTT_CLIENT, "OnSetAlgoTaskInfo ----.");
}

static void OnDelAlgoTaskInfo(AX_U32 id) {
    LOG_M_C(MQTT_CLIENT, "OnDelAlgoTaskInfo ++++.");

    json root;

    // 获取当前通道信息
    AX_U32 nMediaCnt = 0;
    STREAM_CONFIG_T streamConfig = CBoxConfig::GetInstance()->GetStreamConfig();
    std::vector<MEDIA_INFO_T> mediasMap = CBoxMediaParser::GetInstance()->GetMediasMap(&nMediaCnt, streamConfig.strMediaPath);
    if (id < (AX_U32)mediasMap.size()) {
        mediasMap[id].nMediaStatus = 1; // 0异常 1正常/未使用 2使用中
        mediasMap[id].taskInfo.nTaskDelete = 1;
        mediasMap[id].taskInfo.nTaskStatus = 0; // 0未运行 1运行中
        memset(mediasMap[id].taskInfo.szTaskKey, 0, sizeof(mediasMap[id].taskInfo.szTaskKey));

        std::unique_lock<std::mutex> lock(mtx);
        StreamQueue.push({ContrlCmd::RemoveAlgo, id});
        lock.unlock();

        // 更新配置
        CBoxMediaParser::GetInstance()->SetMediasMap(mediasMap);


        root["result"] = 0;
        root["msg"] = "success";
    } else {
        root["result"] = -1;
        root["msg"] = "invalid stream id!";
    }

    json child;
    child["type"] = "delAlgoTaskInfo";

    root["data"] = child;

    std::string payload = root.dump();
    SendMsg("web-message", payload.c_str(), payload.size());

    LOG_M_C(MQTT_CLIENT, "OnDelAlgoTaskInfo ----.");
}

static void OnAlgoTaskControl(AX_U32 id, AX_U32 controlCommand) {
    LOG_M_C(MQTT_CLIENT, "OnAlgoTaskControl ++++.");

    json child;
    child["type"] = "algoTaskControl";

    json root;

    // 获取当前通道信息
    AX_U32 nMediaCnt = 0;
    STREAM_CONFIG_T streamConfig = CBoxConfig::GetInstance()->GetStreamConfig();
    std::vector<MEDIA_INFO_T> mediasMap = CBoxMediaParser::GetInstance()->GetMediasMap(&nMediaCnt, streamConfig.strMediaPath);
    if (id < (AX_U32)mediasMap.size()) {
        if (controlCommand == ContrlCmd::StartAlgo) {
            mediasMap[id].nMediaStatus = 2; // 0异常 1正常/未使用 2使用中
            mediasMap[id].taskInfo.nTaskStatus = 1; // 0未运行 1运行中

            std::unique_lock<std::mutex> lock(mtx);
            StreamQueue.push({static_cast<ContrlCmd>(controlCommand), id});
            lock.unlock();

            // sleep 1 second
            sleep(1);

            AX_CHAR szIP[64] = {0};
            AX_CHAR mediaUrl[128] = {0};

            GetIP(szIP);
            sprintf(mediaUrl, "rtsp://%s:8554/axstream%d", szIP, id + 1);

            // 通知媒体服务器主动拉流
            AX_CHAR api[256] = {0};
            time_t t = time(nullptr);
            sprintf(api, "/index/api/addStreamProxy?secret=%s&vhost=%s&app=%s&stream=%ld&url=%s", ZLM_SECRET, ZLM_IP, "live", t, mediaUrl);
            httplib::Client httpclient(ZLM_API_URL);
            httplib::Logger logger([](const httplib::Request &req, const httplib::Response &res) {
                printf("=====================================================================\n");
                printf("http request path=%s, body=%s\n", req.path.c_str(), req.body.c_str());
                printf("=====================================================================\n");
                printf("http response body=\n%s", res.body.c_str());
                printf("=====================================================================\n"); });
            httpclient.set_logger(logger);
            httplib::Result result = httpclient.Get(api);
            if (result && result->status == httplib::OK_200) {
                auto jsonRes = nlohmann::json::parse(result->body);
                int code = jsonRes["code"];
                if (code != 0) {
                    root["result"] = -1;
                    root["msg"] = jsonRes["msg"];
                } else {
                    std::string key = jsonRes["data"]["key"];
                    strcpy(mediasMap[id].taskInfo.szTaskKey, key.c_str());

                    root["result"] = 0;
                    root["msg"] = "success";
                }
            }
        } else if (controlCommand == ContrlCmd::StopAlgo || controlCommand == ContrlCmd::RemoveAlgo) {
            mediasMap[id].nMediaStatus = 1; // 0异常 1正常/未使用 2使用中
            if (controlCommand == ContrlCmd::StopAlgo) {
                mediasMap[id].nMediaStatus = 2; // 0异常 1正常/未使用 2使用中
            }
            mediasMap[id].taskInfo.nTaskStatus = 0; // 0未运行 1运行中

            // 通知媒体服务器删除流
            char api[256] = {0};
            sprintf(api, "/index/api/delStreamProxy?secret=%s&key=%s", ZLM_SECRET, mediasMap[id].taskInfo.szTaskKey);
            httplib::Client httpclient(ZLM_API_URL);
            httplib::Logger logger([](const httplib::Request &req, const httplib::Response &res) {
                printf("=====================================================================\n");
                printf("http request path=%s, body=%s\n", req.path.c_str(), req.body.c_str());
                printf("=====================================================================\n");
                printf("http response body=\n%s", res.body.c_str());
                printf("=====================================================================\n"); });
            httpclient.set_logger(logger);
            httplib::Result result = httpclient.Get(api);
            if (result && result->status == httplib::OK_200) {
                auto jsonRes = nlohmann::json::parse(result->body);
                int code = jsonRes["code"];
                if (code != 0) {
                    root["result"] = -1;
                    root["msg"] = jsonRes["msg"];
                } else {
                    root["result"] = 0;
                    root["msg"] = "success";

                    std::unique_lock<std::mutex> lock(mtx);
                    StreamQueue.push({static_cast<ContrlCmd>(controlCommand), id});
                    lock.unlock();
                }
            }
        }

        // 更新配置
        CBoxMediaParser::GetInstance()->SetMediasMap(mediasMap);
    }

    root["data"] = child;

    std::string payload = root.dump();

    SendMsg("web-message", payload.c_str(), payload.size());

    LOG_M_C(MQTT_CLIENT, "OnAlgoTaskControl ----.");
}

static void OnAlgoTaskSnapshot(AX_U32 id) {
    LOG_M_C(MQTT_CLIENT, "OnAlgoTaskSnapshot ++++.");

    LOG_M_C(MQTT_CLIENT, "OnAlgoTaskSnapshot ----.");
}

static std::string macAddressToString(const char* mac) {
    std::ostringstream macStr;
    macStr << std::hex << std::setw(2) << std::setfill('0')
           << (int)mac[0] << ":"
           << (int)mac[1] << ":"
           << (int)mac[2] << ":"
           << (int)mac[3] << ":"
           << (int)mac[4] << ":"
           << (int)mac[5];
    return macStr.str();
}

static void getMacAddress(const std::string& ifname, std::string& mac) {
    int sockfd;
    struct ifreq ifr;

    // 创建套接字
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        return;
    }

    // 清零ifreq结构体
    memset(&ifr, 0, sizeof(struct ifreq));

    // 复制接口名称到ifr_name字段
    strncpy(ifr.ifr_name, ifname.c_str(), IFNAMSIZ - 1);

    // 使用ioctl获取MAC地址
    if (ioctl(sockfd, SIOCGIFHWADDR, &ifr) < 0) {
        close(sockfd);
        return;
    }

    // 转换MAC地址
    mac = macAddressToString(ifr.ifr_hwaddr.sa_data);

    // 关闭套接字
    close(sockfd);
}

static int get_gateway(struct in_addr *gw,const char *ifname) {
    int sock = socket(AF_NETLINK, SOCK_RAW, NETLINK_ROUTE);
    if (sock < 0) {
        LOG_E("socket");
        return -1;
    }

    struct {
        struct nlmsghdr nlh;
        struct rtmsg rtm;
    } req;

    memset(&req, 0, sizeof(req));
    req.nlh.nlmsg_len = NLMSG_LENGTH(sizeof(struct rtmsg));
    req.nlh.nlmsg_type = RTM_GETROUTE;
    req.nlh.nlmsg_flags = NLM_F_DUMP | NLM_F_REQUEST;
    req.rtm.rtm_family = AF_INET;

    if (send(sock, &req, req.nlh.nlmsg_len, 0) < 0) {
        perror("send");
        close(sock);
        return -1;
    }

    char buffer[8192];
    int n;
    while ((n = recv(sock, buffer, sizeof(buffer), 0)) > 0) {
        struct nlmsghdr *nlh = (struct nlmsghdr *)buffer;
        for (; NLMSG_OK(nlh, n); nlh = NLMSG_NEXT(nlh, n)) {
            if (nlh->nlmsg_type == NLMSG_DONE)
                break;
            if (nlh->nlmsg_type == NLMSG_ERROR)
                break;

            struct rtmsg *rtm = (struct rtmsg *)NLMSG_DATA(nlh);
            struct rtattr *rta = (struct rtattr *)RTM_RTA(rtm);
            int len = RTM_PAYLOAD(nlh);

            while (RTA_OK(rta, len)) {
                switch (rta->rta_type) {
                case RTA_GATEWAY:
                    gw->s_addr = *((uint32_t *)RTA_DATA(rta));
                    close(sock);
                    return 0;
                }
                rta = RTA_NEXT(rta, len);
            }
            return -1;
        }
    }

    close(sock);
    return 0;
}

static void OnGetAiBoxNetwork() {
    LOG_M_C(MQTT_CLIENT, "OnGetAiBoxNetwork ++++.");

    struct ifaddrs *ifaddr, *ifa;
    int family, s;
    char host[NI_MAXHOST];

    if (getifaddrs(&ifaddr) == -1) {
        return;
    }

    json arr = nlohmann::json::array();
    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
        if (!ifa->ifa_addr)
            continue;

        family = ifa->ifa_addr->sa_family;

        // Check if interface is valid and IPv4
        if (ifa->ifa_name && family == AF_INET &&
            (!strcmp(ifa->ifa_name, "eth0") || !strcmp(ifa->ifa_name, "eth1"))) {
            s = getnameinfo(ifa->ifa_addr, sizeof(struct sockaddr_in), host, NI_MAXHOST, NULL, 0, NI_NUMERICHOST);
            if (s != 0) {
                printf("getnameinfo() failed: %s\n", gai_strerror(s));
                continue;
            }

            LOG_M_C(MQTT_CLIENT, "Interface %s has IP address: %s", ifa->ifa_name, host);

            struct sockaddr_in *sin_netmask = (struct sockaddr_in *)ifa->ifa_netmask;
            struct in_addr gw;
            if (get_gateway(&gw, ifa->ifa_name)) {
                LOG_E("Can't get gateway");
                continue;
            }

            std::string mac;
            getMacAddress(ifa->ifa_name, mac);

            std::string line;
            std::ifstream ifile("/etc/network/interfaces");
            AX_U32 dhcp = 0;
            size_t pos = 0;
            while (std::getline(ifile, line)) {
                if (line.find("static") != std::string::npos && line.find(ifa->ifa_name) != std::string::npos) {
                    dhcp = true;
                    continue;
                }
                if (dhcp && (line.empty() || (line.find("allow-hotplug") != std::string::npos)))
                    dhcp = false;

                if (dhcp) {
                    pos = line.find("dns-nameservers");
                    if (pos != std::string::npos) {
                        dhcp = false;
                        pos += (16 + 3);
                        line = line.substr(pos, line.length() - pos);
                        break;
                    }
                }
            }

            arr.push_back({
                {"name",    ifa->ifa_name},
                {"dhcp",    dhcp},
                {"address", host},
                {"mask",    inet_ntoa(sin_netmask->sin_addr)},
                {"gateway", inet_ntoa(gw)},
                {"dns",     line},
                {"mac",     mac},
                {"status",  0},
            });
        }
    }

    freeifaddrs(ifaddr);

    json child;
    child["type"] = "getAiBoxNetwork";
    child["Interfaces"] = arr;

    json root;
    root["result"] = 0;
    root["msg"] = "success";
    root["data"] = child;

    std::string payload = root.dump();
    SendMsg("web-message", payload.c_str(), payload.size());

    LOG_M_C(MQTT_CLIENT, "OnGetAiBoxNetwork ----.");
}

static void OnSetAiBoxNetwork(const std::string& name, const AX_U32 dhcp, const std::string& address, const std::string& gateway, const std::string& mask, const std::string& dns) {
    LOG_M_C(MQTT_CLIENT, "OnSetAiBoxNetwork ++++.");

    std::vector<std::string> lines;
    std::ifstream ifile("/etc/network/interfaces");
    std::string line;
    while (std::getline(ifile, line)) {
        lines.push_back(line);
    }

    bool found = false;
    std::vector<std::string> newConfig;
    newConfig.push_back("allow-hotplug " + name);

    if (dhcp)
        newConfig.push_back("iface " + name + " inet dhcp");
    else {
        newConfig.push_back("iface " + name + " inet static");
        newConfig.push_back("    address " + address);
        newConfig.push_back("    netmask " + mask);
        newConfig.push_back("    gateway " + gateway);
        newConfig.push_back("    dns-nameservers " + dns);
    }

    for (auto &line : lines) {
        if (line.find("allow-hotplug " + name) != std::string::npos) {
            found = true;
            continue;
        }
        if (found && (line.empty() || (line.find("allow-hotplug") != std::string::npos)))
            found = false;

        if (found)
            continue;

        newConfig.push_back(line);
    }

    lines = newConfig;

    std::ofstream ofile("/etc/network/interfaces");
    for (const auto& line : lines) {
        ofile << line << std::endl;
    }

    json child;
    child["type"] = "setAiBoxNetwork";

    json root;
    root["result"] = 0;
    root["msg"] = "success";
    root["data"] = child;

    std::string payload = root.dump();
    SendMsg("web-message", payload.c_str(), payload.size());

    system("/etc/init.d/networking restart");

    LOG_M_C(MQTT_CLIENT, "OnSetAiBoxNetwork ----.");
}

// /* TODO: need web support file copy, then show in web*/
AX_BOOL MqttClient::SaveJpgFile(QUEUE_T *jpg_info) {
    JPEG_DATA_INFO_T *pJpegInfo = &jpg_info->tJpegInfo;
    AX_U32 nChn = jpg_info->u64UserData & 0xff;
    AX_U32 nAlgoType = (jpg_info->u64UserData >> 8) & 0xff;

    /* Data file parent directory format: </XXX/DEV_XX/YYYY-MM-DD> */
    AX_CHAR szDateBuf[16] = {0};
    CElapsedTimer::GetLocalDate(szDateBuf, 16, '-');

    AX_CHAR szDateDir[128] = {0};
    sprintf(szDateDir, "%s%s/DEV_%02d/%02d/%s", GetExecPath().c_str(), ALARM_IMG_PATH, nChn, nAlgoType, szDateBuf);

    if (CDiskHelper::CreateDir(szDateDir, AX_FALSE)) {
        sprintf(pJpegInfo->tCaptureInfo.tHeaderInfo.szImgPath, "%s/%s_%02d_%02d.jpg", szDateDir, pJpegInfo->tCaptureInfo.tHeaderInfo.szTimestamp, nChn, nAlgoType);

        // Open file to write
        std::ofstream outFile(pJpegInfo->tCaptureInfo.tHeaderInfo.szImgPath, std::ios::binary);
        if (!outFile) {
            std::cerr << "Failed to open file for writing: " << pJpegInfo->tCaptureInfo.tHeaderInfo.szImgPath << std::endl;
            return AX_FALSE;
        }

        // Write the actual data to the file
        outFile.write(reinterpret_cast<const char*>(jpg_info->jpg_buf), jpg_info->buf_length);
        outFile.close();

        LOG_MM_C(MQTT_CLIENT, ">>>> Save jpg file: %s <<<<", pJpegInfo->tCaptureInfo.tHeaderInfo.szImgPath);
    } else {
        LOG_MM_E(MQTT_CLIENT, "[%d][%d] Create date(%s) directory failed.", pJpegInfo->tCaptureInfo.tHeaderInfo.nSnsSrc, pJpegInfo->tCaptureInfo.tHeaderInfo.nChannel, szDateDir);
    }

    return AX_TRUE;
}

AX_VOID MqttClient::SendAlarmMsg() {
    AX_U32 nCount = arrjpegQ->GetCount();
    if (nCount > 0) {
        QUEUE_T jpg_info;
        if (arrjpegQ->Pop(jpg_info, 0)) {
            SaveJpgFile(&jpg_info);
            AX_U32 nChn = jpg_info.u64UserData & 0xff;
            AX_U32 nAlgoType = (jpg_info.u64UserData >> 8) & 0xff;

            std::string currentTimeStr;
            GetSystime(currentTimeStr);

            // 获取当前通道信息
            AX_U32 nMediaCnt = 0;
            STREAM_CONFIG_T streamConfig = CBoxConfig::GetInstance()->GetStreamConfig();
            std::vector<MEDIA_INFO_T> mediasMap = CBoxMediaParser::GetInstance()->GetMediasMap(&nMediaCnt, streamConfig.strMediaPath);

            json child = {
                { "Time", currentTimeStr },
                { "taskName", mediasMap[nChn].taskInfo.szTaskName },
                { "pushUrl", mediasMap[nChn].taskInfo.szPushUrl },
                { "channleId", nChn },
                { "AlarmType", nAlgoType },
                { "jpgPath", jpg_info.tJpegInfo.tCaptureInfo.tHeaderInfo.szImgPath },
            };

            AX_CHAR AudioFile[128] = { 0 };
            sprintf(AudioFile, "%saudio/%d.wav", GetExecPath().c_str(),  nAlgoType);
            if (access(AudioFile, F_OK) == 0) {
                CBoxBuilder *a_builder = CBoxBuilder::GetInstance();
                a_builder->playAudio(AudioFile);
            } else {
                LOG_D("AudioFile %s not exist", AudioFile);
            }

            json root;
            root["result"] = 0;
            root["msg"] = "success";
            root["data"] = child;

            // std::string payload = root.dump();

            // SendMsg("web-message", payload.c_str(), payload.size());
        }
    }
}

static void OnAlarmControl(AX_BOOL isAudio, AX_U32 status) {
    json root;

    if (isAudio) {
        root["type"] = "audio";
        root["result"] = CBoxConfig::GetInstance()->SetAudioValue(status);
    } else {
        root["type"] = "window";
        root["result"] = CBoxConfig::GetInstance()->SetWindowValue(status);
    }

    std::string payload = root.dump();
    SendMsg("web-message", payload.c_str(), payload.size());
}

static void OnGetAlarmStatus(void) {
    json root;
    root["type"] = "alarmStatus";

    DETECT_CONFIG_T detecConfig = CBoxConfig::GetInstance()->GetDetectConfig();
    root["audio"] = detecConfig.bAudio;
    root["window"] = detecConfig.bWindow;

    std::string payload = root.dump();
    SendMsg("web-message", payload.c_str(), payload.size());
}

static AX_VOID removeJpgFile(AX_BOOL isDir, nlohmann::json fileUrls) {
    printf("removeJpgFile ++++\n");

    json root;

    if (isDir) {
        AX_CHAR JpgDir[128] = { 0 };
        sprintf(JpgDir, "%s%s/", GetExecPath().c_str(), ALARM_IMG_PATH);

        root["type"] = "clearAllJpg";
        root["result"] = CDiskHelper::RemoveDir(JpgDir);
    } else {
        root["type"] = "clearJpgFile";
        if (fileUrls.is_array()) {
            for (std::string path : fileUrls) {
                CDiskHelper::RemoveFile(path.c_str());
            }
        }
        root["result"] = 1;
    }

    std::string payload = root.dump();
    SendMsg("web-message", payload.c_str(), payload.size());

    printf("removeJpgFile ----\n");
}

/*保证回调执行的程序要简单，如果比较复杂，需要考虑要用状态机处理回调*/
static void messageArrived(MQTT::MessageData& md) {
    LOG_MM_D(MQTT_CLIENT,"messageArrived ++++\n");

    MQTT::Message &message = md.message;
    LOG_MM_D(MQTT_CLIENT, "Message %d arrived: qos %d, retained %d, dup %d, packetid %d.",
            ++arrivedcount, message.qos, message.retained, message.dup, message.id);
    LOG_MM_D(MQTT_CLIENT, "Payload %.*s.", (int)message.payloadlen, (char*)message.payload);

    std::string recv_msg((char *)message.payload, message.payloadlen);
    LOG_M_C(MQTT_CLIENT, "=============================================================");
    LOG_M_C(MQTT_CLIENT, "recv msg: %s", recv_msg.c_str());
    LOG_M_C(MQTT_CLIENT, "=============================================================");

    std::string type;
    nlohmann::json jsonRes;
    try {
        jsonRes = nlohmann::json::parse(recv_msg);
        type = jsonRes["type"];
    } catch (const nlohmann::json::parse_error& e) {
        std::cerr << "JSON parse error: " << e.what() << std::endl;
        std::cerr << "Received message: " << recv_msg << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "An error occurred: " << e.what() << std::endl;
    }

    if (type == "login") { // 账号登录
        std::string account = jsonRes["account"];
        std::string password = jsonRes["password"];
        OnLogin(account, password);
    } else if (type == "getDashBoardInfo") { // 获取仪表盘信息
        OnGetDashBoardInfo();
    } else if (type == "rebootAiBox") { // 重启盒子
        OnRebootAiBox();
    } else if (type == "restartAppService") { // 重启应用服务
        OnRestartAppService();
    } else if (type == "syncSystemTime") { // 同步系统时间
        AX_S32 year = jsonRes["year"];
        AX_S32 month = jsonRes["month"];
        AX_S32 day = jsonRes["day"];
        AX_S32 hour = jsonRes["hour"];
        AX_S32 minute = jsonRes["minute"];
        AX_S32 second = jsonRes["second"];
        OnSyncSystemTime(year, month, day, hour, minute, second);
    } else if (type == "getMediaChannelList") { // 获取通道列表
        OnGetMediaChannelList();
    } else if (type == "setMediaChannelInfo") { // 设置通道信息
        AX_U32 mediaId = jsonRes["mediaId"];
        std::string mediaUrl = jsonRes["mediaUrl"];
        std::string mediaName = jsonRes["mediaName"];
        std::string mediaDesc = jsonRes["mediaDesc"];
        OnSetMediaChannelInfo(mediaId, mediaUrl, mediaName, mediaDesc);
    } else if (type == "delMediaChannelInfo") { // 删除通道信息
        AX_U32 mediaId = jsonRes["mediaId"];
        OnDelMediaChannelInfo(mediaId);
    } else if (type == "getAiModelList") { // 算法模型列表
        OnGetAiModelList();
    } else if (type == "getAlgoTaskList") { // 算法任务列表
        OnGetAlgoTaskList();
    } else if (type == "setAlgoTaskInfo") { // 配置算法任务
        AX_U32 mediaId = jsonRes["mediaId"];
        std::string pushUrl  = jsonRes["taskPushUrl"];
        std::string taskName = jsonRes["taskName"];
        std::string taskDesc = jsonRes["taskDesc"];

        std::vector<AX_U32> vAlgo;
        nlohmann::json algos = jsonRes["taskAlgos"];
        if (algos.is_array()) {
            for (auto algo : algos) {
                vAlgo.push_back((AX_U32)algo);
            }
        }

        OnSetAlgoTaskInfo(mediaId, pushUrl, taskName, taskDesc, vAlgo);
    } else if (type == "delAlgoTaskInfo") { // 删除算法任务
        AX_U32 mediaId = jsonRes["mediaId"];
        OnDelAlgoTaskInfo(mediaId);
    } else if (type == "algoTaskControl") { // 控制算法启停
        AX_U32 mediaId = jsonRes["mediaId"];
        AX_U32 controlCommand = jsonRes["controlCommand"];
        OnAlgoTaskControl(mediaId, controlCommand);
    } else if (type == "algoTaskSnapshot") { // 参考标底图
        AX_U32 mediaId = jsonRes["mediaId"];
        OnAlgoTaskSnapshot(mediaId);
    } else if (type == "getAiBoxNetwork") { // 盒子网络信息
        OnGetAiBoxNetwork();
    } else if (type == "setAiBoxNetwork") { // 设置盒子网络
        std::string name = jsonRes["name"];
        AX_U32 dhcp = jsonRes["dhcp"];
        std::string address = jsonRes["address"];
        std::string gateway = jsonRes["gateway"];
        std::string mask = jsonRes["mask"];
        std::string dns = jsonRes["dns"];
        OnSetAiBoxNetwork(name, dhcp, address, gateway, mask, dns);
    } else if (type == "playAudio") { // 播放告警音频
        AX_U32 status = jsonRes["status"];
        OnAlarmControl(AX_TRUE, status);
    } else if (type == "showWindow") { // 显示告警弹窗
        AX_U32 status = jsonRes["status"];
        OnAlarmControl(AX_FALSE, status);
    } else if (type == "getAlarmStatus") { // 获取告警配置
        OnGetAlarmStatus();
    } else if (type == "clearAllJpg") { // 删除所有图片
        removeJpgFile(AX_TRUE, NULL);
    } else if (type == "clearJpgFiles") { // 删除指定图片
        nlohmann::json fileUrls = jsonRes["fileUrls"];
        removeJpgFile(AX_FALSE, fileUrls);
    }

    LOG_MM_D(MQTT_CLIENT,"messageArrived ----\n");
}


//回调不能出现耗时过久
AX_BOOL MqttClient::OnRecvData(OBS_TARGET_TYPE_E eTarget, AX_U32 nGrp, AX_U32 nChn, AX_VOID* pData) {
    if (E_OBS_TARGET_TYPE_JENC == eTarget) {
        AX_VENC_PACK_T* pVencPack = &((AX_VENC_STREAM_T*)pData)->stPack;
        if (nullptr == pVencPack->pu8Addr || 0 == pVencPack->u32Len) {
            LOG_M_E(MQTT_CLIENT, "Invalid Jpeg data(chn=%d, buff=0x%08X, len=%d).", nChn, pVencPack->pu8Addr, pVencPack->u32Len);
            return AX_FALSE;
        }

        QUEUE_T jpg_info;
        jpg_info.jpg_buf = new AX_U8[MAX_BUF_LENGTH];
        jpg_info.buf_length = pVencPack->u32Len;
        memcpy(jpg_info.jpg_buf, pVencPack->pu8Addr, jpg_info.buf_length);
        jpg_info.u64UserData = pVencPack->u64UserData;

        auto &tJpegInfo = jpg_info.tJpegInfo;
        tJpegInfo.tCaptureInfo.tHeaderInfo.nSnsSrc = nGrp;
        tJpegInfo.tCaptureInfo.tHeaderInfo.nChannel = nChn;
        tJpegInfo.tCaptureInfo.tHeaderInfo.nWidth = 1920;
        tJpegInfo.tCaptureInfo.tHeaderInfo.nHeight = 1080;
        CElapsedTimer::GetLocalTime(tJpegInfo.tCaptureInfo.tHeaderInfo.szTimestamp, 16, '-', AX_FALSE);

        arrjpegQ->Push(jpg_info);
    }

    return AX_TRUE;
}


AX_BOOL MqttClient::Init(MQTT_CONFIG_T &mqtt_config) {
    ipstack_ = std::make_unique<IPStack>();
    client_ = std::make_unique<MQTT::Client<IPStack, Countdown>>(*ipstack_);
    topic = mqtt_config.topic;

    // //实现加锁队列，主要是多线程
    arrjpegQ = std::make_unique<CAXLockQ<QUEUE_T>>();
    if (!arrjpegQ) {
        LOG_MM_E(MQTT_CLIENT, "alloc queue fail");
        return AX_FALSE;
    } else {
        arrjpegQ->SetCapacity(10);
    }

    LOG_M_C(MQTT_CLIENT, "Mqtt Version is %d, topic is %s", mqtt_config.version, topic.c_str());
    LOG_M_C(MQTT_CLIENT, "Connecting to %s:%d", mqtt_config.hostname.c_str(), mqtt_config.port);

    int rc = ipstack_->connect(mqtt_config.hostname.c_str(), mqtt_config.port);
    if (rc != 0) {
        LOG_M_E(MQTT_CLIENT, "rc from TCP connect fail, rc = %d", rc);
        return AX_FALSE;
    } else {
        MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
        data.MQTTVersion = mqtt_config.version;
        data.clientID.cstring = mqtt_config.name.c_str();
        data.username.cstring = mqtt_config.name.c_str();
        data.password.cstring = mqtt_config.passwd.c_str();
        rc = client_->connect(data);
        if (rc != 0) {
            LOG_M_E(MQTT_CLIENT, "rc from MQTT connect fail, rc is %d\n", rc);
            return AX_FALSE;
        } else {
            LOG_M_D(MQTT_CLIENT, "MQTT connected sucess");
            rc = client_->subscribe(topic.c_str(), MQTT::QOS0, messageArrived);
            if (rc != 0) {
                LOG_M_E(MQTT_CLIENT, "rc from MQTT subscribe is %d\n", rc);
                return AX_FALSE;
            }
        }
    }

    return AX_TRUE;
}

AX_BOOL MqttClient::DeInit(AX_VOID) {

    int rc = client_->unsubscribe(topic.c_str());
    if (rc != 0)
        LOG_M_E(MQTT_CLIENT, "rc from unsubscribe was %d", rc);

    rc = client_->disconnect();
    if (rc != 0)
        LOG_M_E(MQTT_CLIENT, "rc from disconnect was %d", rc);

    ipstack_->disconnect();

    LOG_M_C(MQTT_CLIENT, "Finishing with messages received");

    return AX_TRUE;
}

AX_BOOL MqttClient::Start(AX_VOID) {

    if (!m_threadWork.Start([this](AX_VOID* pArg) -> AX_VOID { WorkThread(pArg); }, nullptr, "MQTT_CLIENT")) {
        LOG_MM_E(MQTT_CLIENT, "Create ai switch thread fail.");
        return AX_FALSE;
    }

    return AX_TRUE;
}

AX_BOOL MqttClient::Stop(AX_VOID) {
    if (arrjpegQ) {
        arrjpegQ->Wakeup();
    }

    m_threadWork.Stop();
    m_threadWork.Join();

    return AX_TRUE;
}

AX_VOID MqttClient::WorkThread(AX_VOID* pArg) {
    LOG_MM_I(MQTT_CLIENT, "+++");

    CBoxBuilder *p_builder = CBoxBuilder::GetInstance();
    while (m_threadWork.IsRunning()) {
        // process alarm message
        SendAlarmMsg();

        if (!StreamQueue.empty()) {
            std::unique_lock<std::mutex> lock(mtx);
            StreamCmd stream_cmd = StreamQueue.front();
            StreamQueue.pop();
            lock.unlock();

            if (stream_cmd.cmd == ContrlCmd::StartAlgo) {
                p_builder->RemoveStream(stream_cmd.id);
                p_builder->AddStream(stream_cmd.id);
            } else if (stream_cmd.cmd == ContrlCmd::RemoveAlgo) {
                p_builder->RemoveStream(stream_cmd.id);
            } else if (stream_cmd.cmd == ContrlCmd::StopAlgo) {
                p_builder->RemoveStream(stream_cmd.id);
            }
        }

        client_->yield(50UL); // sleep 50ms
    }

    LOG_MM_I(MQTT_CLIENT, "---");
}

};
