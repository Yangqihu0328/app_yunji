#include "MqttClient.hpp"

#include "cmdline.hpp"
#include "AppLog.hpp"
#include "DiskHelper.hpp"
#include "ElapsedTimer.hpp"
#include "BoxBuilder.hpp"
#include "EncoderOptionHelper.h"
#include "BoxMediaParser.hpp"

#include "httplib.h"
#include "nlohmann/json.hpp"

namespace boxconf {

#define MQTT_CLIENT "MQTT_CLIENT"

#define ZLM_IP      "127.0.0.1"
#define ZLM_API_URL "http://" ZLM_IP ":80"
#define ZLM_SECRET  "81DBE7AF-ACD5-47D8-A692-F4B27456E6FD"

#define ALARM_IMG_PATH "/mnt/nfs/ZLMediaKit/www/alarm"

using namespace std;
using json = nlohmann::json;

//需要转为类成员变量，提高代码简洁性
static std::shared_ptr<IPStack> ipstack_ = nullptr;
static std::shared_ptr<MQTT::Client<IPStack, Countdown>> client_ = nullptr;
static json board_info;
static int arrivedcount = 0;
static std::vector<MediaChanel> media_channel;
static std::vector<AlgoTask> algo_task;
// static std::map<int, int> id_channel_map;
static std::queue<int> addIdQueue;
static std::queue<int> RemoveIdQueue;
static std::mutex mtx_info;
static std::mutex mtx;

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

int GetIP(const std::string interfaceName, std::string &ip_addr, std::string &net_type) {
    struct ifaddrs *ifaddr, *ifa;
    int family, s;
    char host[NI_MAXHOST];

    if (getifaddrs(&ifaddr) == -1) {
        perror("getifaddrs");
        return -1;
    }

    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
        if (!ifa->ifa_addr) 
            continue;

        family = ifa->ifa_addr->sa_family;

        // Check if interface is valid and IPv4
        if ((family == AF_INET) && (strcmp(ifa->ifa_name, interfaceName.c_str()) == 0)) {
            s = getnameinfo(ifa->ifa_addr, sizeof(struct sockaddr_in), host, NI_MAXHOST, NULL, 0, NI_NUMERICHOST);
            if (s != 0) {
                LOG_M_C(MQTT_CLIENT, "getnameinfo() failed: %s\n", gai_strerror(s));
                continue;
            }
            
            LOG_M_C(MQTT_CLIENT, "Interface %s has IP address: %s", interfaceName.c_str(), host);
            ip_addr = std::string(host);
            net_type = "WAN";
            freeifaddrs(ifaddr);
            return 0;
        }
    }

    freeifaddrs(ifaddr);
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
            LOG_M_C(MQTT_CLIENT, "MQTT CLIENT publish [%s] message [%s] failed ...", topic, msg);
        }
    } else {
        LOG_M_C(MQTT_CLIENT, "MQTT CLIENT can not get client to publis");
    }

    LOG_M_C(MQTT_CLIENT, "SendMsg ----.");

    return true;
}

static void OnLogin(const std::string& accout, const std::string& password) {
    LOG_M_C(MQTT_CLIENT, "OnLogin ++++.");

    LOG_M_C(MQTT_CLIENT, "OnLogin ----.");
}

static void OnGetDashBoardInfo() {
    LOG_M_C(MQTT_CLIENT, "OnGetDashBoardInfo ++++.");

    int temperature = -1;
    int ret = GetTemperature(temperature);
    if (ret == -1) {
        LOG_MM_D(MQTT_CLIENT, "GetTemperature fail.");
    }
    
    const std::string interfaceName = "eth0";
    std::string ipAddress = "127.0.0.1";
    std::string NetType = "LAN";
    ret = GetIP(interfaceName, ipAddress, NetType);
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
        {"type", "GetBoardInfo"}, 
        {"BoardId", "YJ-AIBOX-001"}, 
        {"BoardIp", ipAddress},
        {"BoardPlatform", "AX650"},
        {"BoardTemp", temperature},
        {"BoardType", NetType},
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

    LOG_M_C(MQTT_CLIENT, "OnRestartAppService ----.");
}

// // 查找下一个可用的 ID
// static int findNextAvailableId() {
//     std::set<int> existing_ids;

//     // 收集所有已存在的 ID
//     for (const auto& channel : media_channel) {
//         existing_ids.insert(channel.id);
//     }

//     // 从 0 开始查找第一个未被使用的 ID
//     int next_id = 0;
//     while (existing_ids.find(next_id) != existing_ids.end()) {
//         next_id++;
//     }
//     return next_id;
// }

// 这个函数可能会有耗时问题
bool check_RTSP_stream(const std::string& rtspUrl) {
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
        // 测试连接
        AX_U32 status = check_RTSP_stream(mediasMap[i].szMediaUrl) ? 1 : 0;

        arr.push_back({
            {"mediaId",     mediasMap[i].nMediaId},
            {"mediaStatus", status},
            {"mediaUrl",    mediasMap[i].szMediaUrl},
            {"mediaName",   mediasMap[i].szMediaName},
            {"mediaDesc",   mediasMap[i].szMediaDesc},
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

    // 测试连接
    AX_U32 status = check_RTSP_stream(mediaUrl) ? 1 : 0;

    json root;

    // 获取当前通道信息
    AX_U32 nMediaCnt = 0;
    STREAM_CONFIG_T streamConfig = CBoxConfig::GetInstance()->GetStreamConfig();
    std::vector<MEDIA_INFO_T> mediasMap = CBoxMediaParser::GetInstance()->GetMediasMap(&nMediaCnt, streamConfig.strMediaPath);
    if (id < (AX_U32)mediasMap.size()) {
        mediasMap[id].nMediaId = id;
        mediasMap[id].nMediaStatus = status;
        strcpy(mediasMap[id].szMediaUrl, mediaUrl.c_str());
        strcpy(mediasMap[id].szMediaName, mediaName.c_str());
        strcpy(mediasMap[id].szMediaDesc, mediaDesc.c_str());

        // 更新配置
        CBoxMediaParser::GetInstance()->SetMediasMap(mediasMap);

        std::unique_lock<std::mutex> lock(mtx);
        addIdQueue.push(id);
        lock.unlock();

        root["result"] = 0;
        root["msg"] = "success";
    } else {
        root["result"] = -1;
        root["msg"] = "invalid stream id!";
    }

    json child;
    child["type"] = "setMediaChannelInfo";
    child["status"] = status; // 0异常 1正常

    root["data"] = child;

    std::string payload = root.dump();
    SendMsg("web-message", payload.c_str(), payload.size());

    LOG_M_C(MQTT_CLIENT, "OnSetMediaChannelInfo ----.");

    // //这里应该判断id数量限制为0-15
    // if (media_channel.size() > 16 || id<0 || id>16) {
    //      root["msg"] = "failure";
    //      LOG_M_D(MQTT_CLIENT, "media channel only smaller than 16");
    // } else {
    //     //从media_channel中遍历找到最小的id进行赋值
    //     int next_id = findNextAvailableId();
    //     if (id != next_id) {
    //         root["msg"] = "failure";
    //     } else {
    //         //这里应该只是判断一下流能否正常取图，而不会开启功能。
    //         MediaChanel newchannel;
    //         if (check_RTSP_stream(url)) {
    //             newchannel.channel_status = "accessible";
    //         } else {
    //             newchannel.channel_status = "fail";
    //         }
    //         newchannel.id = id;
    //         newchannel.url = url;
    //         media_channel.push_back(newchannel);

    //         CBoxConfig::GetInstance()->AddStreamUrl(id, url);
    //         root["msg"] = "success";
    //     }
    // }

    // root["data"] = child;
    // std::string payload = root.dump();
    // SendMsg("web-message", payload.c_str(), payload.size());
}

static void OnDelMediaChannelInfo(AX_U32 id) {
    LOG_M_C(MQTT_CLIENT, "OnDelMediaChannelInfo ++++.");

    json root;

    // 获取当前通道信息
    AX_U32 nMediaCnt = 0;
    STREAM_CONFIG_T streamConfig = CBoxConfig::GetInstance()->GetStreamConfig();
    std::vector<MEDIA_INFO_T> mediasMap = CBoxMediaParser::GetInstance()->GetMediasMap(&nMediaCnt, streamConfig.strMediaPath);
    if (id < (AX_U32)mediasMap.size()) {
        mediasMap[id].nMediaDisable = 1;

        // 更新配置
        CBoxMediaParser::GetInstance()->SetMediasMap(mediasMap);

        std::unique_lock<std::mutex> lock(mtx);
        RemoveIdQueue.push(id);
        lock.unlock();

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

    // auto it = std::find_if(media_channel.begin(), media_channel.end(),
    //     [id](const MediaChanel& channel) { return channel.id == id; });

    // if (it != media_channel.end()) {
    //     media_channel.erase(it);

    //     CBoxConfig::GetInstance()->RemoveStreamUrl(id);
    //     root["msg"] = "success";
    // } else {
    //     root["msg"] = "failure";
    // }
}

static void OnGetAlgoTaskList() {
    LOG_M_C(MQTT_CLIENT, "OnGetAlgoTaskList ++++.");

    // json root, child;
    // child["type"] = "QueryAlgoTask";

    // // 媒体通道包括:媒体通道的id，媒体的视频源,视频协议，以及配置了什么算法。
    // // media_channel 是vector,需要转为json格式发送出去。
    // json channels_array = json::array();
    // for (const auto& task : algo_task) {
    //     json task_info;
    //     task_info["id"] = task.id;
    //     task_info["url"] = task.url;
    //     task_info["algo1"] = task.algo_index0;
    //     task_info["algo2"] = task.algo_index1;
    //     task_info["algo3"] = task.algo_index2;
    //     task_info["channel_status"] = task.channel_status;
    //     channels_array.push_back(task_info); // 将当前通道的信息添加到数组中
    // }
    // child["arry"] = channels_array;

    // root["msg"] = "success";
    // root["data"] = child;
    // std::string payload = root.dump();
    // SendMsg("web-message", payload.c_str(), payload.size());

    LOG_M_C(MQTT_CLIENT, "OnGetAlgoTaskList ----.");
}

//TODO:后面要增加算法其他信息，例如越线检测的线的位置或者客流统计的人员数量阈值设置
static void OnSetAlgoTaskInfo(int id) {
    LOG_M_C(MQTT_CLIENT, "OnSetAlgoTaskInfo ++++.");

    // json child, root;
    // child["type"] = "AddAlgoTask";
    // root["result"] = 0;

    // if (algo_task.size() > 16) {
    //      root["msg"] = "failure";
    // } else {
    //     //任务这里是不管id的，因为任务是与url相绑定的
    //     AlgoTask temp_algo_task;
    //     temp_algo_task.id = id;
    //     temp_algo_task.url = url;
    //     temp_algo_task.channel_status = "ready";
    //     temp_algo_task.algo_index0 = algo_vec[0];
    //     temp_algo_task.algo_index1 = algo_vec[1];
    //     temp_algo_task.algo_index2 = algo_vec[2];

    //     algo_task.push_back(temp_algo_task);
    //     //应该算法任务的id也是跟媒体通道用同一个id才对，这样子才是合理。
    //     CBoxConfig::GetInstance()->AddAlgoTask(id, algo_vec);
    //     //没有配置算法任务的时候，不应该开始媒体通道。
    //     // std::unique_lock<std::mutex> lock(mtx);
    //     // addIdQueue.push(id);
    //     // lock.unlock();
    //     root["msg"] = "success";
    // }

    // root["data"] = child;
    // std::string payload = root.dump();
    // SendMsg("web-message", payload.c_str(), payload.size());

    LOG_M_C(MQTT_CLIENT, "OnSetAlgoTaskInfo ----.");
}

static void OnDelAlgoTaskInfo(int id) {
    LOG_M_C(MQTT_CLIENT, "OnDelAlgoTaskInfo ++++.");

    // json child;
    // child["type"] = "RemoveMediaChanel";

    // json root;
    // root["result"] = 0;

    // auto it = std::find_if(algo_task.begin(), algo_task.end(),
    //     [id](const AlgoTask& channel) { return channel.id == id; });

    // if (it != algo_task.end()) {
    //     algo_task.erase(it);
    //     std::unique_lock<std::mutex> lock(mtx);
    //     RemoveIdQueue.push(id);
    //     lock.unlock();
    //     CBoxConfig::GetInstance()->RemoveAlgoTask(id);

    //     root["msg"] = "success";
    // } else {
    //     root["msg"] = "failure";
    // }

    // root["data"] = child;
    // std::string payload = root.dump();
    // SendMsg("web-message", payload.c_str(), payload.size());

    LOG_M_C(MQTT_CLIENT, "OnDelAlgoTaskInfo ----.");
}

static void OnAlgoTaskControl(AX_U32 id, AX_U32 controlCommand) {
    LOG_M_C(MQTT_CLIENT, "OnAlgoTaskControl ++++.");

    LOG_M_C(MQTT_CLIENT, "OnAlgoTaskControl ----.");
}

static void OnAlgoTaskSnapshot(AX_U32 id) {
    LOG_M_C(MQTT_CLIENT, "OnAlgoTaskSnapshot ++++.");

    LOG_M_C(MQTT_CLIENT, "OnAlgoTaskSnapshot ----.");
}

static void OnGetAiBoxNetwork() {
    LOG_M_C(MQTT_CLIENT, "OnGetAiBoxNetwork ++++.");

    LOG_M_C(MQTT_CLIENT, "OnGetAiBoxNetwork ----.");
}

static void OnSetAiBoxNetwork(const std::string& name, const std::string& address, const std::string& gateway, const std::string& mask, const std::string& dns) {
    LOG_M_C(MQTT_CLIENT, "OnGetAiBoxNetwork ++++.");

    LOG_M_C(MQTT_CLIENT, "OnGetAiBoxNetwork ----.");
}

static void OnStartRtspPreview(std::string& streamUrl) {
    printf("OnStartRtspPreview ++++\n");

    time_t t = time(nullptr);

    httplib::Client httpclient(ZLM_API_URL);
    httplib::Logger logger([](const httplib::Request &req, const httplib::Response &res) {
        printf("=====================================================================\n");
        printf("http request path=%s, body=%s\n", req.path.c_str(), req.body.c_str());
        printf("=====================================================================\n");
        printf("http response body=\n%s", res.body.c_str());
        printf("=====================================================================\n"); });
    httpclient.set_logger(logger);

    char api[128] = {0};
    sprintf(api, "/index/api/addStreamProxy?secret=%s&vhost=%s&app=%s&stream=%ld&url=%s", ZLM_SECRET, ZLM_IP, "live", t, streamUrl.c_str());

    json child;
    child["type"] = "startRtspPreview";
    child["streamUrl"] = streamUrl;

    json root;
    httplib::Result result = httpclient.Get(api);
    if (result && result->status == httplib::OK_200) {
        auto jsonRes = nlohmann::json::parse(result->body);
        int code = jsonRes["code"];
        if (code == 0) {
            std::string key = jsonRes["data"]["key"];
            child["key"] = key;

            root["result"] = 0;
            root["msg"] = "success";
        } else {
            root["result"] = -1;
            root["msg"] = jsonRes["msg"];
        }
    }
    root["data"] = child;

    std::string payload = root.dump();

    SendMsg("web-message", payload.c_str(), payload.size());

    printf("OnStartRtspPreview ----\n");
}

static void OnStopRtspPreview(std::string& key) {
    printf("OnStopRtspPreview ++++\n");

    httplib::Client httpclient(ZLM_API_URL);
    httplib::Logger logger([](const httplib::Request &req, const httplib::Response &res) {
        printf("=====================================================================\n");
        printf("http request path=%s, body=%s\n", req.path.c_str(), req.body.c_str());
        printf("=====================================================================\n");
        printf("http response body=\n%s", res.body.c_str());
        printf("=====================================================================\n"); });
    httpclient.set_logger(logger);

    char api[128] = {0};
    sprintf(api, "/index/api/delStreamProxy?secret=%s&key=%s", ZLM_SECRET, key.c_str());

    json child;
    child["type"] = "stopRtspPreview";

    json root;
    root["data"] = child;
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
        }
    }

    std::string payload = root.dump();

    SendMsg("web-message", payload.c_str(), payload.size());

    printf("OnStopRtspPreview ----\n");
}

// /* TODO: need web support file copy, then show in web*/
AX_BOOL MqttClient::SaveJpgFile(QUEUE_T *jpg_info) {
    JPEG_DATA_INFO_T *pJpegInfo = &jpg_info->tJpegInfo;

    /* Data file parent directory format: </XXX/DEV_XX/YYYY-MM-DD> */
    AX_CHAR szDateBuf[16] = {0};
    CElapsedTimer::GetLocalDate(szDateBuf, 16, '-');

    AX_CHAR szDateDir[128] = {0};
    sprintf(szDateDir, "%s/DEV_%02d/%s", ALARM_IMG_PATH, pJpegInfo->tCaptureInfo.tHeaderInfo.nSnsSrc + 1, szDateBuf);

    if (CDiskHelper::CreateDir(szDateDir, AX_FALSE)) {
        sprintf(pJpegInfo->tCaptureInfo.tHeaderInfo.szImgPath, "%s/%s_%lld.jpg", szDateDir, pJpegInfo->tCaptureInfo.tHeaderInfo.szTimestamp, jpg_info->u64UserData);

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

            std::string currentTimeStr;
            GetSystime(currentTimeStr);

            AX_CHAR szDateBuf[16] = {0};
            CElapsedTimer::GetLocalDate(szDateBuf, 16, '-');

            AX_CHAR szDateDir[128] = {0};
            sprintf(szDateDir, "%s/DEV_%02d/%s", ALARM_IMG_PATH, jpg_info.tJpegInfo.tCaptureInfo.tHeaderInfo.nSnsSrc + 1, szDateBuf);

            AX_CHAR szImgPath[256] = {0};
            sprintf(szImgPath, "%s/%s.jpg", szDateDir, jpg_info.tJpegInfo.tCaptureInfo.tHeaderInfo.szTimestamp);

            json child = {
                {"type", "alarmMsg"},    {"BoardId", "YJ-AIBOX-001"}, {"Time", currentTimeStr},
                {"AlarmType", "people"}, {"AlarmStatus", "success"},  {"AlarmContent", "alarm test test ..."},
                {"Path", szImgPath},  // jpg_info.tJpegInfo.tCaptureInfo.tHeaderInfo.szImgPath},
                {"channleId", 1},     // jpg_info.tJpegInfo.tCaptureInfo.tHeaderInfo.nChannel},
            };

            json root;
            root["result"] = 0;
            root["msg"] = "success";
            root["data"] = child;

            std::string payload = root.dump();

            SendMsg("web-message", payload.c_str(), payload.size());
        }
    }
}

/*保证回调执行的程序要简单，如果比较复杂，需要考虑要用状态机处理回调*/
static void messageArrived(MQTT::MessageData& md) {
    LOG_MM_D(MQTT_CLIENT,"messageArrived ++++\n");

    MQTT::Message &message = md.message;
    LOG_MM_D(MQTT_CLIENT, "Message %d arrived: qos %d, retained %d, dup %d, packetid %d.",
            ++arrivedcount, message.qos, message.retained, message.dup, message.id);
    LOG_MM_D(MQTT_CLIENT, "Payload %.*s.", (int)message.payloadlen, (char*)message.payload);

    std::string recv_msg((char *)message.payload, message.payloadlen);
    printf("recv msg: %s\n", recv_msg.c_str());

    std::string type;
    nlohmann::json jsonRes;
    try {
        jsonRes = nlohmann::json::parse(recv_msg);
        type = jsonRes["type"];
        printf("msg type %s\n", type.c_str());
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
    } else if (type == "getAlgoTaskList") { // 算法任务列表
        OnGetAlgoTaskList();
    } else if (type == "setAlgoTaskInfo") { // 配置算法任务
        AX_U32 mediaId = jsonRes["mediaId"];
        OnSetAlgoTaskInfo(mediaId);
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
        std::string address = jsonRes["address"];
        std::string gateway = jsonRes["gateway"];
        std::string mask = jsonRes["mask"];
        std::string dns = jsonRes["dns"];
        OnSetAiBoxNetwork(name, address, gateway, mask, dns);
    } else if (type == "startRtspPreview") { // 开始预览
        std::string mediaUrl = jsonRes["mediaUrl"];
        OnStartRtspPreview(mediaUrl);
    } else if (type == "stopRtspPreview") { // 停止预览
        std::string mediaUrl = jsonRes["mediaUrl"];
        OnStopRtspPreview(mediaUrl);
    } 

    // if (recv_msg == "add0") {
    //     // std::string mstring = "rtsp://admin:yunji123456++@192.168.0.150:554/cam/realmonitor?channel=1&subtype=0";
    //     std::string mstring = "/root/boxDemo/3.mp4";
    //     OnAddMediaChanel(0, mstring);

    //     std::unique_lock<std::mutex> lock(mtx);
    //     addIdQueue.push(0);
    //     lock.unlock();
    //     // std::vector<int> algo_vec = {4, 4, 4};
    //     // OnAddAlgoTask(0, mstring, algo_vec);
    // }

    // if (recv_msg == "add1") {
    //     // std::string mstring = "rtsp://admin:yunji123456++@192.168.0.150:554/cam/realmonitor?channel=1&subtype=0";
    //     std::string mstring = "/root/boxDemo/4.mp4";
    //     OnAddMediaChanel(1, mstring);

    //     std::unique_lock<std::mutex> lock(mtx);
    //     addIdQueue.push(1);
    //     lock.unlock();
    //     // std::vector<int> algo_vec = {4, 4, 4};
    //     // OnAddAlgoTask(0, mstring, algo_vec);
    // }

    // if (recv_msg == "remove0") {
    //     OnRemoveMediaChanel(0);
    //     std::unique_lock<std::mutex> lock(mtx);
    //     RemoveIdQueue.push(0);
    //     lock.unlock();
    // }

    // if (recv_msg == "remove1") {
    //     OnRemoveMediaChanel(1);
    //     std::unique_lock<std::mutex> lock(mtx);
    //     RemoveIdQueue.push(1);
    //     lock.unlock();
    // }

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

    int id = 0;

    while (m_threadWork.IsRunning()) {
        // process alarm message
        SendAlarmMsg();

        //不加队列可能会错过
        std::unique_lock<std::mutex> lock(mtx);
        auto p_builder = CBoxBuilder::GetInstance();
        if (!addIdQueue.empty()) {
            id = addIdQueue.front();
            p_builder->StopStream(id);
            p_builder->StartStream(id);
            addIdQueue.pop();
        } else if (!RemoveIdQueue.empty()) {
            id = RemoveIdQueue.front();
            p_builder->StopStream(id);
            RemoveIdQueue.pop();
        }
        lock.unlock();
        
        client_->yield(1 * 1000UL); // sleep 1 seconds
    }

    LOG_MM_I(MQTT_CLIENT, "---");
}

};
