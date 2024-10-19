#include "MqttClient.hpp"

#include "cmdline.hpp"
#include "AppLog.hpp"
// #include "AiSwitchConfig.hpp"
#include "DiskHelper.hpp"
#include "ElapsedTimer.hpp"
// #include "AiCardMstBuilder.hpp"

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
static std::mutex mtx;
static json board_info;
static int arrivedcount = 0;
static std::vector<MediaChanel> media_channel;
static std::vector<AlgoTask> algo_task;

// #if 0

// CTransferHelper* m_pTransferHelper = nullptr;



// static std::map<std::string, std::string> gb28181_discovery_;

// std::vector<AX_BOOL> stream_status_;


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
                printf("getnameinfo() failed: %s\n", gai_strerror(s));
                continue;
            }
            
            LOG_M_D(MQTT_CLIENT, "Interface %s has IP address: %s", interfaceName.c_str(), host);
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
            LOG_M_D(MQTT_CLIENT, "Cannot open the file.");
            return -1;
        }

        enable_file << "1";
        enable_file.close();
        read_flag = 1;
    }

    std::ifstream temp_file("/proc/ax_proc/npu/top");
    if (!temp_file) {
        LOG_M_D(MQTT_CLIENT, "Cannot open temperature file.");
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
    LOG_M_D(MQTT_CLIENT, "endMsg ++++.");

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
            LOG_M_D(MQTT_CLIENT, "MQTT CLIENT publish [%s] message [%s] failed ...", topic, msg);
        }
    } else {
        LOG_M_D(MQTT_CLIENT, "MQTT CLIENT can not get client to publis");
    }

    LOG_M_D(MQTT_CLIENT, "SendMsg ----.");

    return true;
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


static bool IsMediaChannelRegistered(int id) {
    return std::any_of(mediaChannels.begin(), mediaChannels.end(),
                       [id](const MediaChanel& channel) {
                           return channel.id == id;
                       });
}

static bool IsAlgoTaskRegistered(int id) {
    return std::any_of(algo_task.begin(), algo_task.end(),
                       [id](const AlgoTask& task) {
                           return task.id == id;
                       });
}

//应该不考虑顺序也没关系
static void OnAddMediaChanel(int id, std::string url) {
    json child;
    child["type"] = "AddMediaChanel";
    json root;
    root["result"] = 0;

    if (media_channel.size() > 16) {
         root["msg"] = "failure";
    } else {
        //判断是否注册过
        if (IsMediaChannelRegistered) {
            root["msg"] = "failure";
        } else {
            MediaChanel newchannel;
            newchannel.id = id;
            newchannel.url = url;
            newchannel.channel_status = "ready";

            media_channel.push_back(newchannel);
            
            //TODO: add realy media channel; meanwhile confirm rtsp stream is ok? add channel_status attribute

            root["msg"] = "success";
        }
    }

    root["data"] = child;
    std::string payload = root.dump();
    SendMsg("web-message", payload.c_str(), payload.size());
}

//TODO:后面要增加算法其他信息，例如越线检测的线的位置或者客流统计的人员数量阈值设置
static void OnAddAlgoTask(int task_id, std::string url, std::vector<std::string> algo_vec) {
    json child, root;
    child["type"] = "AddAlgoTask";
    root["result"] = 0;

    if (algo_task.size() > 16) {
         root["msg"] = "failure";
    } else {
        if (IsAlgoTaskRegistered) {
            root["msg"] = "failure";
        } else {
            AlgoTask temp_algo_task;
            temp_algo_task.id = id;
            temp_algo_task.url = url;
            temp_algo_task.channel_status = "ready";
            temp_algo_task.algo_index0 = algo_vec[0];
            temp_algo_task.algo_index1 = algo_vec[1];
            temp_algo_task.algo_index2 = algo_vec[2];

            algo_task.push_back(temp_algo_task);
            //TODO: add realy media channel; meanwhile confirm rtsp stream is ok? add channel_status attribute

            root["msg"] = "success";
        }
        
    }

    root["data"] = child;
    std::string payload = root.dump();
    SendMsg("web-message", payload.c_str(), payload.size());
}

static void OnRemoveMediaChanel(int id) {
    json child;
    child["type"] = "RemoveMediaChanel";

    json root;
    root["result"] = 0;

    auto it = std::find_if(mediaChannels.begin(), mediaChannels.end(),
        [id](const MediaChanel& channel) { return channel.id == id; });

    if (it != mediaChannels.end()) {
        mediaChannels.erase(it);

        root["msg"] = "success";
    } else {
        root["msg"] = "failure";
    }
    //TODO: remove realy media channel;

    root["data"] = child;
    std::string payload = root.dump();
    SendMsg("web-message", payload.c_str(), payload.size());
}

static void OnRemoveAlgoTask(int id) {
    json child;
    child["type"] = "RemoveMediaChanel";

    json root;
    root["result"] = 0;

    auto it = std::find_if(algo_task.begin(), algo_task.end(),
        [id](const MediaChanel& channel) { return channel.id == id; });

    if (it != algo_task.end()) {
        algo_task.erase(it);

        root["msg"] = "success";
    } else {
        root["msg"] = "failure";
    }
    //TODO: remove realy algo task;

    root["data"] = child;
    std::string payload = root.dump();
    SendMsg("web-message", payload.c_str(), payload.size());
}

//查询媒体通道的目的就是为了确定是否取流成功。
static void OnQueryMediaChanel() {
    LOG_M_D(MQTT_CLIENT, "OnQueryMediaChanel ++++.");


    json root, child;
    child["type"] = "QueryMediaChanel";

    // 媒体通道包括:媒体通道的id，媒体的视频源,视频协议，以及配置了什么算法。
    // media_channel 是vector,需要转为json格式发送出去。
    for (const auto channel : media_channel) {
        child["id"] = channel.id;
        child["url"] = channel.url;
        child["channel_status"] = channel.channel_status;
    }
    root["msg"] = "success";
    root["data"] = child;
    std::string payload = root.dump();
    SendMsg("web-message", payload.c_str(), payload.size());
    LOG_M_D(MQTT_CLIENT, "OnQueryMediaChanel ----.");
}

static void OnQueryAlgoTask() {
    LOG_M_D(MQTT_CLIENT, "OnQueryAlgoTask ++++.");


    json root, child;
    child["type"] = "QueryMediaChanel";

    // 媒体通道包括:媒体通道的id，媒体的视频源,视频协议，以及配置了什么算法。
    // media_channel 是vector,需要转为json格式发送出去。
    for (const auto task : algo_task) {
        child["id"] = task.id;
        child["url"] = task.url;
        child["algo1"] = task.algo_index0;
        child["algo2"] = task.algo_index1;
        child["algo3"] = task.algo_index2;
        child["channel_status"] = channel.channel_status;
    }
    root["msg"] = "success";
    root["data"] = child;
    std::string payload = root.dump();
    SendMsg("web-message", payload.c_str(), payload.size());
    LOG_M_D(MQTT_CLIENT, "OnQueryAlgoTask ----.");
}

// static void OnRestartMasterApp() {
//     printf("OnRestartMaster ++++\n");

//     json child;
//     child["type"] = "restartMasterApp";

//     json root;
//     root["result"] = 0;
//     root["msg"] = "success";
//     root["data"] = child;

//     std::string payload = root.dump();

//     SendMsg("web-message", payload.c_str(), payload.size());

//     printf("OnRestartMaster ----\n");
// }


static void OnGetBoardInfo() {
    std::lock_guard<std::mutex> lock(mtx);
    json root;
    root["result"] = 0;
    root["msg"] = "success";
    root["data"] = board_info;
    
    std::string payload = root.dump();
    SendMsg("web-message", payload.c_str(), payload.size());
}

/*循环中获取板子信息，减少回调耗时*/
static void GetBoardInfo() {
    LOG_MM_D(MQTT_CLIENT, "OnGetBoardInfo ++++");

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

    std::lock_guard<std::mutex> lock(mtx);
    board_info = {
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

    LOG_MM_D(MQTT_CLIENT, "OnGetBoardInfo ----");
}


// static void OnGetMediaChannelInfo() {
//     printf("OnMediaChannelInfo ++++\n");
//     json arr = nlohmann::json::array();
//     STREAM_CONFIG_T streamConfig = CAiCardMstConfig::GetInstance()->GetStreamConfig();

//     // init stream status
//     if (0 == stream_status_.size()) {
//         const AX_U32 nCount = streamConfig.v.size();
//         printf(">>>> init stream status vector size: %u <<<<\n", nCount);

//         stream_status_.resize(nCount);
//         for (size_t i = 0; i < stream_status_.size(); i++) {
//             stream_status_[i] = AX_TRUE;
//         }
//     }

//     for (size_t i = 0; i < streamConfig.v.size(); i++) {
//         arr.push_back({
//             {"channelId", i}, 
//             {"channelName", i + 1}, 
//             {"channelStatus", stream_status_[i]==AX_TRUE ? 1 : 0}, 
//             {"protocol", "rtsp"}, 
//             {"channelUrl", streamConfig.v[i]}
//         });
//     }

//     int device_count = 0;
//     auto itr = gb28181_discovery_.begin();
// 	for (; itr != gb28181_discovery_.end(); ++itr) {
//         arr.push_back({
//             {"channelId", device_count}, 
//             {"channelName", device_count + 1}, 
//             {"channelStatus", 1}, 
//             {"protocol", "gb28181"}, 
//             {"channelUrl", itr->second}
//         });
//         device_count++;
// 	}

//     json child;
//     child["type"] = "getMediaChannelInfo";
//     child["channels"] = arr;

//     json root;
//     root["result"] = 0;
//     root["msg"] = "success";
//     root["data"] = child;

//     std::string payload = root.dump();

//     SendMsg("web-message", payload.c_str(), payload.size());

//     printf("OnMediaChannelInfo ----\n");
// }

// static void OnSetMediaChannelInfo(int channelId, std::string& streamUrl) {
//     printf("OnSetMediaChannelInfo ++++\n");
//     STREAM_CONFIG_T streamConfig = CAiCardMstConfig::GetInstance()->GetStreamConfig();

//     json child;
//     child["type"] = "setMediaChannelInfo";

//     json root;
//     root["data"] = child;
//     if (channelId < (int)streamConfig.v.size()) {
//         streamConfig.v[channelId] = streamUrl;

//         // update ai card mst config stream url
//         if (CAiCardMstConfig::GetInstance()->SetStreamUrl(channelId + 1, streamUrl)) {
//             if (CAiCardMstBuilder::GetInstance()->StopStream(channelId)) {
//                 if (CAiCardMstBuilder::GetInstance()->StartStream(channelId)) {
//                     stream_status_[channelId] = AX_TRUE;

//                     root["result"] = 0;
//                     root["msg"] = "success";
//                 } else {
//                     root["result"] = -1;
//                     root["msg"] = "start stream failed!";
//                 }
//             } else {
//                 root["result"] = -1;
//                 root["msg"] = "stop stream failed!";
//             }
//         } else {
//             root["result"] = -1;
//             root["msg"] = "set stream url failed!";
//         }
//     } else {
//         root["result"] = -1;
//         root["msg"] = "invalid channel id";
//     }

//     std::string payload = root.dump();

//     SendMsg("web-message", payload.c_str(), payload.size());

//     printf("OnSetMediaChannelInfo ----\n");
// }

// static void OnDelMediaChannelInfo(int channelId) {
//     printf("OnDelMediaChannelInfo ++++\n");
//     STREAM_CONFIG_T streamConfig = CAiCardMstConfig::GetInstance()->GetStreamConfig();

//     json child;
//     child["type"] = "delMediaChannelInfo";

//     json root;
//     root["data"] = child;
//     if (channelId < (int)streamConfig.v.size()) {
//         if (CAiCardMstBuilder::GetInstance()->StopStream(channelId)) {
//             stream_status_[channelId] = AX_FALSE;

//             root["result"] = 0;
//             root["msg"] = "success";
//         } else {
//             root["result"] = -1;
//             root["msg"] = "stop stream failed!";
//         }
//     } else {
//         root["result"] = -1;
//         root["msg"] = "invalid channel id";
//     }

//     std::string payload = root.dump();

//     SendMsg("web-message", payload.c_str(), payload.size());

//     printf("OnDelMediaChannelInfo ----\n");
// }


// static void OnGetMediaChannelTaskInfo() {
//     printf("OnMediaChannelTaskInfo ++++\n");

//     json child;
//     child["type"] = "getMediaChannelTaskInfo";

//     json root;
//     root["result"] = 0;
//     root["msg"] = "success";
//     root["data"] = child;

//     std::string payload = root.dump();

//     SendMsg("web-message", payload.c_str(), payload.size());

//     printf("OnMediaChannelTaskInfo ----\n");
// }

// static void OnGetAiModelList() {
//     printf("OnGetAiModelList ++++\n");

//     json arr = nlohmann::json::array();
//     arr.push_back({
//         {"modelId", 0}, 
//         {"modePath", "/opt/etc/skelModels/1024x576/part/ax_ax650_hvcfp_algo_model_V1.0.2.axmodel"}, 
//         {"modeName", "人车非"}, 
//     });
//     arr.push_back({
//         {"modelId", 1}, 
//         {"modePath", "/opt/etc/skelModels/1024x576/part/ax_ax650_fire_algo_model_V1.0.2.axmodel"}, 
//         {"modeName", "火焰检测"}, 
//     });

//     json child;
//     child["type"] = "getAiModelList";
//     child["models"] = arr;

//     json root;
//     root["result"] = 0;
//     root["msg"] = "success";
//     root["data"] = child;

//     std::string payload = root.dump();

//     SendMsg("web-message", payload.c_str(), payload.size());

//     printf("OnGetAiModelList ----\n");
// }

// static void OnSwitchChannelAiModel() {
//     printf("OnwitchChannelAiModel ++++\n");

//     json child;
//     child["type"] = "switchChannelAiModel";

//     AI_CARD_AI_SWITCH_ATTR_T tAiAttr;
//     CAiSwitchConfig::GetInstance()->GetNextAttr(tAiAttr);
//     if (m_pTransferHelper != nullptr) {
//         m_pTransferHelper->SendAiAttr(tAiAttr);
//     }

//     json root;
//     root["result"] = 0;
//     root["msg"] = "success";
//     root["data"] = child;

//     std::string payload = root.dump();

//     SendMsg("web-message", payload.c_str(), payload.size());

//     printf("OnwitchChannelAiModel ----\n");
// }

// static void OnStartRtspPreview(std::string& streamUrl) {
//     printf("OnStartRtspPreview ++++\n");

//     time_t t = time(nullptr);

//     httplib::Client httpclient(ZLM_API_URL);
//     httplib::Logger logger([](const httplib::Request &req, const httplib::Response &res) {
//         printf("=====================================================================\n");
//         printf("http request path=%s, body=%s\n", req.path.c_str(), req.body.c_str());
//         printf("=====================================================================\n");
//         printf("http response body=\n%s", res.body.c_str());
//         printf("=====================================================================\n"); });
//     httpclient.set_logger(logger);

//     char api[128] = {0};
//     sprintf(api, "/index/api/addStreamProxy?secret=%s&vhost=%s&app=%s&stream=%ld&url=%s", ZLM_SECRET, ZLM_IP, "live", t, streamUrl.c_str());

//     json child;
//     child["type"] = "startRtspPreview";
//     child["streamUrl"] = streamUrl;

//     json root;
//     httplib::Result result = httpclient.Get(api);
//     if (result && result->status == httplib::OK_200) {
//         auto jsonRes = nlohmann::json::parse(result->body);
//         int code = jsonRes["code"];
//         if (code == 0) {
//             std::string key = jsonRes["data"]["key"];
//             child["key"] = key;

//             root["result"] = 0;
//             root["msg"] = "success";
//         } else {
//             root["result"] = -1;
//             root["msg"] = jsonRes["msg"];
//         }
//     }
//     root["data"] = child;

//     std::string payload = root.dump();

//     SendMsg("web-message", payload.c_str(), payload.size());

//     printf("OnStartRtspPreview ----\n");
// }

// static void OnStopRtspPreview(std::string& key) {
//     printf("OnStopRtspPreview ++++\n");

//     httplib::Client httpclient(ZLM_API_URL);
//     httplib::Logger logger([](const httplib::Request &req, const httplib::Response &res) {
//         printf("=====================================================================\n");
//         printf("http request path=%s, body=%s\n", req.path.c_str(), req.body.c_str());
//         printf("=====================================================================\n");
//         printf("http response body=\n%s", res.body.c_str());
//         printf("=====================================================================\n"); });
//     httpclient.set_logger(logger);

//     char api[128] = {0};
//     sprintf(api, "/index/api/delStreamProxy?secret=%s&key=%s", ZLM_SECRET, key.c_str());

//     json child;
//     child["type"] = "stopRtspPreview";

//     json root;
//     root["data"] = child;
//     httplib::Result result = httpclient.Get(api);
//     if (result && result->status == httplib::OK_200) {
//         auto jsonRes = nlohmann::json::parse(result->body);
//         int code = jsonRes["code"];
//         if (code != 0) {
//             root["result"] = -1;
//             root["msg"] = jsonRes["msg"];
//         } else {
//             root["result"] = 0;
//             root["msg"] = "success";
//         }
//     }

//     std::string payload = root.dump();

//     SendMsg("web-message", payload.c_str(), payload.size());

//     printf("OnStopRtspPreview ----\n");
// }




// AX_VOID MqttClient::BindTransfer(CTransferHelper* pInstance) {
//     m_pTransferHelper = pInstance;
// }


// /* TODO: need web support file copy, then show in web*/
// AX_BOOL MqttClient::SaveJpgFile(AX_VOID* data, AX_U32 size, JPEG_DATA_INFO_T* pJpegInfo) {
//     std::lock_guard<std::mutex> guard(m_mtxConnStatus);

//     /* Data file parent directory format: </XXX/DEV_XX/YYYY-MM-DD> */
//     AX_CHAR szDateBuf[16] = {0};
//     CElapsedTimer::GetLocalDate(szDateBuf, 16, '-');

//     AX_CHAR szDateDir[128] = {0};
//     sprintf(szDateDir, "%s/DEV_%02d/%s", ALARM_IMG_PATH, pJpegInfo->tCaptureInfo.tHeaderInfo.nSnsSrc + 1, szDateBuf);

//     if (CDiskHelper::CreateDir(szDateDir, AX_FALSE)) {
//         sprintf(pJpegInfo->tCaptureInfo.tHeaderInfo.szImgPath, "%s/%s.jpg", szDateDir, pJpegInfo->tCaptureInfo.tHeaderInfo.szTimestamp);

//         // Open file to write
//         std::ofstream outFile(pJpegInfo->tCaptureInfo.tHeaderInfo.szImgPath, std::ios::binary);
//         if (!outFile) {
//             std::cerr << "Failed to open file for writing: " << pJpegInfo->tCaptureInfo.tHeaderInfo.szImgPath << std::endl;
//             return AX_FALSE;
//         }

//         // Write the actual data to the file
//         outFile.write(reinterpret_cast<const char*>(data), size);
//         outFile.close();

//         LOG_MM_C(MQTT_CLIENT, ">>>> Save jpg file: %s <<<<", pJpegInfo->tCaptureInfo.tHeaderInfo.szImgPath);
//     } else {
//         LOG_MM_E(MQTT_CLIENT, "[%d][%d] Create date(%s) directory failed.", pJpegInfo->tCaptureInfo.tHeaderInfo.nSnsSrc, pJpegInfo->tCaptureInfo.tHeaderInfo.nChannel, szDateDir);
//     }

//     return AX_TRUE;
// }

// AX_VOID MqttClient::SendAlarmMsg(MQTT::Message &message) {
//     AX_U32 nCount = arrjpegQ->GetCount();
//     if (nCount > 0) {
//         QUEUE_T jpg_info;
//         if (arrjpegQ->Pop(jpg_info, 0)) {
//             SaveJpgFile(jpg_info.jpg_buf, jpg_info.buf_length, &(jpg_info.tJpegInfo));

//             std::string currentTimeStr;
//             MqttGetSystime(currentTimeStr);

//             AX_CHAR szDateBuf[16] = {0};
//             CElapsedTimer::GetLocalDate(szDateBuf, 16, '-');

//             AX_CHAR szDateDir[128] = {0};
//             sprintf(szDateDir, "%s/DEV_%02d/%s", ALARM_IMG_PATH, jpg_info.tJpegInfo.tCaptureInfo.tHeaderInfo.nSnsSrc + 1, szDateBuf);

//             AX_CHAR szImgPath[256] = {0};
//             sprintf(szImgPath, "%s/%s.jpg", szDateDir, jpg_info.tJpegInfo.tCaptureInfo.tHeaderInfo.szTimestamp);

//             json child = {
//                 {"type", "alarmMsg"},    {"BoardId", "YJ-AIBOX-001"}, {"Time", currentTimeStr},
//                 {"AlarmType", "people"}, {"AlarmStatus", "success"},  {"AlarmContent", "alarm test test ..."},
//                 {"Path", szImgPath},  // jpg_info.tJpegInfo.tCaptureInfo.tHeaderInfo.szImgPath},
//                 {"channleId", 1},     // jpg_info.tJpegInfo.tCaptureInfo.tHeaderInfo.nChannel},
//             };

//             json root;
//             root["result"] = 0;
//             root["msg"] = "success";
//             root["data"] = child;

//             std::string payload = root.dump();

//             SendMsg("web-message", payload.c_str(), payload.size());
//         }
//     }
// }
// #endif


/*保证回调执行的程序要简单，如果比较复杂，需要考虑要用状态机处理回调*/
static void messageArrived(MQTT::MessageData& md) {
    printf("messageArrived ++++\n");

    MQTT::Message &message = md.message;
    LOG_MM_D(MQTT_CLIENT, "Message %d arrived: qos %d, retained %d, dup %d, packetid %d.",
            ++arrivedcount, message.qos, message.retained, message.dup, message.id);
    LOG_MM_D(MQTT_CLIENT, "Payload %.*s.", (int)message.payloadlen, (char*)message.payload);

    std::string recv_msg((char *)message.payload, message.payloadlen);
    printf("recv msg %s\n", recv_msg.c_str());

    auto jsonRes = nlohmann::json::parse(recv_msg);
    std::string type = jsonRes["type"];
    printf("msg type %s\n", type.c_str());



    // if (type == "login") {
    //     std::string account  = jsonRes["account"];
    //     std::string password = jsonRes["password"];
    //     OnLogin(account, password);
    // } else if (type == "rebootAiBox") {
    //     OnRebootAiBox();
    // } else if (type == "restartMasterApp") {
    //     OnRestartMasterApp();

    if (type == "rebootAiBox") {
        OnRebootAiBox();
    } else if (type == "getBoardInfo") {
        OnGetBoardInfo();
    } else if (type == "AddMediaChanel") {
        int id  = jsonRes["channelId"];
        std::string url  = jsonRes["url"];
        OnAddMediaChanel(id, url);
    } else if (type == "RemoveMediaChanel") {
        int id  = jsonRes["channelId"];
        OnRemoveMediaChanel(id);
    } else if (type == "QueryMediaChanel") {
        OnQueryMediaChanel();
    } else if (type == "AddAlgoTask") {
        int TaskId  = jsonRes["TaskId"];
        std::string url  = jsonRes["url"];
        std::vector<std::string> algo_vec;
        algo_vec.push_back(jsonRes["algo1"]);
        algo_vec.push_back(jsonRes["algo2"]);
        algo_vec.push_back(jsonRes["algo3"]);
        OnAddAlgoTask(id, url, algo_vec);
    } else if (type == "RemoveAlgoTask") {
        int id  = jsonRes["channelId"];
        OnRemoveAlgoTask(id);
    } else if (type == "QueryAlgoTask") {
        OnQueryAlgoTask();
    }
    // #if 0
    // else if (type == "getMediaChannelInfo") {
    //     OnGetMediaChannelInfo();
    // } else if (type == "setMediaChannelInfo") {
    //     int channelId  = jsonRes["channelId"];
    //     std::string streamUrl = jsonRes["streamUrl"];
    //     OnSetMediaChannelInfo(channelId, streamUrl);
    // } else if (type == "delMediaChannelInfo") {
    //     int channelId  = jsonRes["channelId"];
    //     OnDelMediaChannelInfo(channelId);
    // } else if (type == "getMediaChannelTaskInfo") {
    //     OnGetMediaChannelTaskInfo();
    // } 
    // #endif
    // else if (type == "getAiModelList") {
    //     OnGetAiModelList();
    // } else if (type == "switchChannelAiModel") {
    //     OnSwitchChannelAiModel();
    // }  else if (type == "startRtspPreview") {
    //     std::string streamUrl = jsonRes["streamUrl"];
    //     OnStartRtspPreview(streamUrl);
    // } else if (type == "stopRtspPreview") {
    //     std::string key = jsonRes["key"];
    //     OnStopRtspPreview(key);
    // } else if (type == "gb28181-discovery") { // add gb28181 stream
    //     std::string deviceId = jsonRes["deviceId"];
    //     std::string streamUrl = jsonRes["streamUrl"];
    //     gb28181_discovery_[deviceId] = streamUrl;
    // } else if (type == "gb28181b-bye") { // delete gb28181 stream
    //     std::string deviceId = jsonRes["deviceId"];
    //     if (gb28181_discovery_.count(deviceId) > 0) {
    //         gb28181_discovery_.erase(deviceId);
    //     }
    // }

    printf("messageArrived ----\n");
}


//回调不能出现耗时过久
AX_BOOL MqttClient::OnRecvData(OBS_TARGET_TYPE_E eTarget, AX_U32 nGrp, AX_U32 nChn, AX_VOID* pData) {
    // if (E_OBS_TARGET_TYPE_JENC == eTarget) {
    //     AX_VENC_PACK_T* pVencPack = &((AX_VENC_STREAM_T*)pData)->stPack;
    //     if (nullptr == pVencPack->pu8Addr || 0 == pVencPack->u32Len) {
    //         LOG_M_E(MQTT_CLIENT, "Invalid Jpeg data(chn=%d, buff=0x%08X, len=%d).", nChn, pVencPack->pu8Addr, pVencPack->u32Len);
    //         return AX_FALSE;
    //     }

    //     QUEUE_T jpg_info;
    //     jpg_info.jpg_buf = new AX_U8[MAX_BUF_LENGTH];
    //     jpg_info.buf_length = pVencPack->u32Len;
    //     memcpy(jpg_info.jpg_buf, pVencPack->pu8Addr, jpg_info.buf_length);

    //     auto &tJpegInfo = jpg_info.tJpegInfo;
    //     tJpegInfo.tCaptureInfo.tHeaderInfo.nSnsSrc = nGrp;
    //     tJpegInfo.tCaptureInfo.tHeaderInfo.nChannel = nChn;
    //     tJpegInfo.tCaptureInfo.tHeaderInfo.nWidth = 1920;
    //     tJpegInfo.tCaptureInfo.tHeaderInfo.nHeight = 1080;
    //     CElapsedTimer::GetLocalTime(tJpegInfo.tCaptureInfo.tHeaderInfo.szTimestamp, 16, '-', AX_FALSE);

    //     arrjpegQ->Push(jpg_info);
    // }

    return AX_TRUE;
}


AX_BOOL MqttClient::Init(MQTT_CONFIG_T &mqtt_config) {
    ipstack_ = std::make_unique<IPStack>();
    client_ = std::make_unique<MQTT::Client<IPStack, Countdown>>(*ipstack_);
    topic = mqtt_config.topic;

    // //实现加锁队列，主要是多线程
    // arrjpegQ = std::make_unique<CAXLockQ<QUEUE_T>>();
    // if (!arrjpegQ) {
    //     LOG_MM_E(MQTT_CLIENT, "alloc queue fail");
    //     return AX_FALSE;
    // } else {
    //     arrjpegQ->SetCapacity(10);
    // }

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
    // if (arrjpegQ) {
    //     arrjpegQ->Wakeup();
    // }

    m_threadWork.Stop();
    m_threadWork.Join();

    return AX_TRUE;
}




AX_VOID MqttClient::WorkThread(AX_VOID* pArg) {
    LOG_MM_I(MQTT_CLIENT, "+++");

    while (m_threadWork.IsRunning()) {
        GetBoardInfo();
        // process alarm message
        // MQTT::Message alarm_message;
        // SendAlarmMsg(alarm_message);
        client_->yield(5 * 1000UL); // sleep 5 seconds
    }

    LOG_MM_I(MQTT_CLIENT, "---");
}

};
