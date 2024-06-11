
#include "AppLog.hpp"
#include "MqttClientServer.hpp"
#include "nlohmann/json.hpp"
#include "AiSwitchConfig.hpp"

#define MQTT_CLIENT "MQTT_CLIENT"

using namespace std;
using json = nlohmann::json;

static int arrivedcount = 0;
CTransferHelper* m_pTransferHelper = nullptr;

static void messageArrived(MQTT::MessageData& md) {
    MQTT::Message &message = md.message;
    LOG_MM_D(MQTT_CLIENT, "Message %d arrived: qos %d, retained %d, dup %d, packetid %d.",
            ++arrivedcount, message.qos, message.retained, message.dup, message.id);
    LOG_MM_D(MQTT_CLIENT, "Payload %.*s.", (int)message.payloadlen, (char*)message.payload);
    char * msg =  (char*)message.payload;
    if (!strcmp(msg, "swtich_next_algo")) {
        AI_CARD_AI_SWITCH_ATTR_T tAiAttr;
        CAiSwitchConfig::GetInstance()->GetNextAttr(tAiAttr);
        if (m_pTransferHelper != nullptr) {
            m_pTransferHelper->SendAiAttr(tAiAttr);
        }
    }
}


static int MqttGetTemperature(int &temp) {

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


static int MqttGetVersion(std::string& version) {

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

static int MqttGetSystime(std::string& timeString) {
    auto now = std::chrono::system_clock::now();
    std::time_t currentTime = std::chrono::system_clock::to_time_t(now);
    std::tm* localTime = std::localtime(&currentTime);

    std::ostringstream oss;
    oss << std::put_time(localTime, "%Y-%m-%d %H:%M:%S");
    timeString = oss.str();
    return 0;
}


int MqttGetIP(const std::string interfaceName, std::string &ip_addr) {
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
            freeifaddrs(ifaddr);
            return 0;
        }
    }

    freeifaddrs(ifaddr);
    return -1;
}


static int MqttMemoryInfo(MemoryInfo &memInfo) {
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
    }
    meminfoFile.close();
    return 0;
}

static int getDiskUsage(const std::string& path, FlashInfo &falsh_info) {
    struct statvfs stat;

    if (statvfs(path.c_str(), &stat) != 0) {
        // 错误处理
        perror("statvfs");
        return -1;
    }

    falsh_info.total = stat.f_blocks * stat.f_frsize;
    falsh_info.free = stat.f_bfree * stat.f_frsize;
    return 0;
}

AX_VOID MqttClientServer::BindTransfer(CTransferHelper* pInstance) {
    m_pTransferHelper = pInstance;
}

//回调不能出现耗时过久
AX_BOOL MqttClientServer::OnRecvData(OBS_TARGET_TYPE_E eTarget, AX_U32 nGrp, AX_U32 nChn, AX_VOID* pData) {
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

        auto &tJpegInfo = jpg_info.tJpegInfo;
        tJpegInfo.tCaptureInfo.tHeaderInfo.nSnsSrc = nGrp;
        tJpegInfo.tCaptureInfo.tHeaderInfo.nChannel = nChn;
        tJpegInfo.tCaptureInfo.tHeaderInfo.nWidth = 1920;
        tJpegInfo.tCaptureInfo.tHeaderInfo.nHeight = 1080;

        arrjpegQ->Push(jpg_info);
    }
    return AX_TRUE;
}

/* TODO: need web support file copy, then show in web*/
AX_BOOL MqttClientServer::SaveJpgFile(AX_VOID* data, AX_U32 size, JPEG_DATA_INFO_T* pJpegInfo) {
    std::lock_guard<std::mutex> guard(m_mtxConnStatus);

    LOG_MM_D(MQTT_CLIENT, "[%d] Send capture data, size=%d", size);
    static int count = 0;

    // Construct file name based on sensor ID, channel, and timestamp
    std::string fileName = "capture_" + std::to_string(count) + ".jpg";
    count++;
    // Open file to write
    std::ofstream outFile(fileName, std::ios::binary);
    if (!outFile) {
        std::cerr << "Failed to open file for writing: " << fileName << std::endl;
        return AX_FALSE;
    }

    // Write the actual data to the file
    outFile.write(reinterpret_cast<const char*>(data), size);
    outFile.close();

    LOG_MM_D(MQTT_CLIENT, "Data captured and saved to %s", fileName);
    return AX_TRUE;
}

AX_VOID MqttClientServer::SendBoardHeart(MQTT::Message &message) {
    int temperature = -1;
    int ret = MqttGetTemperature(temperature);
    if (ret == -1) {
        LOG_MM_D(MQTT_CLIENT, "MqttGetTemperature fail.");
    }
    
    const std::string interfaceName = "eth0";
    std::string ipAddress;
    ret = MqttGetIP(interfaceName, ipAddress);
    if (ret == -1) {
        LOG_MM_D(MQTT_CLIENT, "MqttGetIP fail.");
    }

    MemoryInfo memInfo = {0};
    ret = MqttMemoryInfo(memInfo);
    if (ret == -1) {
        LOG_MM_D(MQTT_CLIENT, "MqttMemoryInfo fail.");
    }

    FlashInfo falsh_info = {0};
    ret = getDiskUsage("/", falsh_info);

    std::string currentTimeStr;
    MqttGetSystime(currentTimeStr);

    std::string version;
    MqttGetVersion(version);

    json board_heart = {
        {"BoardId", "YJ-AIBOX-001"}, 
        {"BoardIp", ipAddress},
        {"BoardPlatform", "AX650"},
        {"BoardTemp", temperature},
        {"BoardType", "LAN"},
        {"GBClientId", ""},
        {"GBClientId", "已授权"},
        {"Time", currentTimeStr},
        {"Version", version},
        {"Medias", { // 当前设备添加的通道信息（0.0.46+引入）
            { 
                {"MediaName", "1"}, // 通道名称
                {"MediaStatus", { // 通道状态描述
                    {"label", "正常"}, 
                    {"type", 4}
                }},
                {"MediaUrl", "rtsp://192.168.0.64:8556/h264"}, // 通道视频流地址
                {"SubId", ""} // 通道关联 GB28181 通道号，配置后透传
            }
        }},
        {"HostDisk", { // 当前设备硬盘情况 kB
            {"Total", falsh_info.total}, // 总量
            {"Available", falsh_info.free} // 已用
        }},
        {"HostMemory", { // 当前设备内存使用情况
            {"Total", memInfo.totalMem}, // 总量
            {"Available", memInfo.availableMem}, // 总量
        }},
        {"Tpu", { // 当前设备的算力资源使用情况（0.0.46+引入）
            { 
                {"mem_total", 7983}, // 内存总量
                {"mem_usage", 376}, // 内存使用情况
                {"tpu_usage", 6} // TPU 使用情况
            }
        }},
    };

    std::string payload = board_heart.dump();
    message.qos = MQTT::QOS0;
    message.retained = false;
    message.dup = false;
    message.payload = (void*)payload.c_str();
    message.payloadlen = payload.length();
    
    int rc = client->publish(topic.c_str(), message);
    if (rc != 0)
        LOG_MM_D(MQTT_CLIENT, "Error %d from sending QoS 0 message", rc);
    LOG_MM_D(MQTT_CLIENT, "topic is %s", topic.c_str());
    //must modify packet type
    client->yield(1*1000);
}

AX_VOID MqttClientServer::SendAlarmMsg(MQTT::Message &message) {
    std::string currentTimeStr;
    MqttGetSystime(currentTimeStr);

    json alarm_msg = {
        {"BoardId", "YJ-AIBOX-001"}, 
        {"Alarm", "people"},
        {"Time", currentTimeStr},
    };

    AX_U32 nCount = arrjpegQ->GetCount();
    if (nCount > 0) {
        QUEUE_T jpg_info;
        if (arrjpegQ->Pop(jpg_info, 0)) {
            SaveJpgFile(jpg_info.jpg_buf, jpg_info.buf_length, &(jpg_info.tJpegInfo));
        }
    }

    std::string payload = alarm_msg.dump();
    message.qos = MQTT::QOS0;
    message.retained = false;
    message.dup = false;
    message.payload = (void*)payload.c_str();
    message.payloadlen = payload.length();
    
    int rc = client->publish(topic.c_str(), message);
    if (rc != 0)
        LOG_MM_D(MQTT_CLIENT, "Error %d from sending QoS 0 message", rc);
    LOG_MM_D(MQTT_CLIENT, "topic is %s", topic.c_str());
    //must modify packet type
    client->yield(5*1000);
}

AX_BOOL MqttClientServer::Init(MQTT_CONFIG_T &mqtt_config) {
    ipstack = std::make_unique<IPStack>();
    client = std::make_unique<MQTT::Client<IPStack, Countdown>>(*ipstack);
    topic = mqtt_config.topic;
    sub_topic = mqtt_config.sub_topic;

    //实现加锁队列，主要是多线程
    arrjpegQ = std::make_unique<CAXLockQ<QUEUE_T>>();
    if (!arrjpegQ) {
        LOG_MM_E(MQTT_CLIENT, "alloc queue fail");
        return AX_FALSE;
    } else {
        arrjpegQ->SetCapacity(10);
    }

    LOG_M_D(MQTT_CLIENT, "Mqtt Version is %d, topic is %s", 
            mqtt_config.version, topic.c_str());
    LOG_M_D(MQTT_CLIENT, "Connecting to %s:%d", 
            mqtt_config.hostname.c_str(), mqtt_config.port);

    int rc = ipstack->connect(mqtt_config.hostname.c_str(), mqtt_config.port);
	if (rc != 0) {
        LOG_M_E(MQTT_CLIENT, "rc from TCP connect fail, rc = %d", rc);
    } else {
        MQTTPacket_connectData data = MQTTPacket_connectData_initializer;       
        data.MQTTVersion = mqtt_config.version;
        data.clientID.cstring = mqtt_config.client_name.c_str();
        rc = client->connect(data);
        if (rc != 0) {
            LOG_M_E(MQTT_CLIENT, "rc from MQTT connect fail, rc is %d\n", rc);
        } else {
            LOG_M_D(MQTT_CLIENT, "MQTT connected sucess");
            rc = client->subscribe(sub_topic.c_str(), MQTT::QOS2, messageArrived);   
            if (rc != 0)
                LOG_M_E(MQTT_CLIENT, "rc from MQTT subscribe is %d\n", rc);
        }
    }

    return AX_TRUE;
}

AX_BOOL MqttClientServer::DeInit(AX_VOID) {


    int rc = client->unsubscribe(topic.c_str());
    if (rc != 0)
        LOG_M_E(MQTT_CLIENT, "rc from unsubscribe was %d", rc);

    rc = client->disconnect();
    if (rc != 0)
        LOG_M_E(MQTT_CLIENT, "rc from disconnect was %d", rc);

    ipstack->disconnect();
    LOG_M_E(MQTT_CLIENT, "Finishing with messages received");
    
    return AX_TRUE;
}

AX_BOOL MqttClientServer::Start(AX_VOID) {

    if (!m_threadWork.Start([this](AX_VOID* pArg) -> AX_VOID { WorkThread(pArg); }, nullptr, "MQTT_CLIENT")) {
        LOG_MM_E(MQTT_CLIENT, "Create ai switch thread fail.");
        return AX_FALSE;
    }

    return AX_TRUE;
}

AX_BOOL MqttClientServer::Stop(AX_VOID) {
    if (arrjpegQ) {
        arrjpegQ->Wakeup();
    }

    m_threadWork.Stop();
    m_threadWork.Join();

    return AX_TRUE;
}

AX_VOID MqttClientServer::WorkThread(AX_VOID* pArg) {
    LOG_MM_E(MQTT_CLIENT, "+++");

    while (m_threadWork.IsRunning()) {
        //every average 3 second send board heart
        MQTT::Message hear_message;
        SendBoardHeart(hear_message);
        
        //process alarm message
        MQTT::Message alarm_message;
        SendAlarmMsg(alarm_message);
    }

    LOG_MM_E(MQTT_CLIENT, "---");
}
