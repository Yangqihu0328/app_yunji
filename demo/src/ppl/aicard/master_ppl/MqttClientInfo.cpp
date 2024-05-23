
#include "AppLog.hpp"
#include "MqttClientInfo.hpp"
#include "nlohmann/json.hpp"

#define MQTT_CLIENT "MQTT_CLIENT"

using namespace std;
using json = nlohmann::json;

static int arrivedcount = 0;
static void messageArrived(MQTT::MessageData& md) {
    MQTT::Message &message = md.message;

    printf("Message %d arrived: qos %d, retained %d, dup %d, packetid %d\n", 
		++arrivedcount, message.qos, message.retained, message.dup, message.id);
    printf("Payload %.*s\n", (int)message.payloadlen, (char*)message.payload);
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


AX_BOOL MqttClientInfo::Init(MQTT_CONFIG_T &mqtt_config) {
    ipstack = std::make_unique<IPStack>();
    client = std::make_unique<MQTT::Client<IPStack, Countdown>>(*ipstack);
    topic = mqtt_config.topic;
    sub_topic = mqtt_config.sub_topic;

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

AX_BOOL MqttClientInfo::DeInit(AX_VOID) {
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

AX_BOOL MqttClientInfo::Start(AX_VOID) {

    if (!m_threadWork.Start([this](AX_VOID* pArg) -> AX_VOID { WorkThread(pArg); }, nullptr, "MQTT_CLIENT")) {
        LOG_MM_E(MQTT_CLIENT, "Create ai switch thread fail.");
        return AX_FALSE;
    }

    return AX_TRUE;
}

AX_BOOL MqttClientInfo::Stop(AX_VOID) {
    m_threadWork.Stop();
    m_threadWork.Join();

    return AX_TRUE;
}

AX_VOID MqttClientInfo::WorkThread(AX_VOID* pArg) {
    LOG_MM_E(MQTT_CLIENT, "+++");

    while (m_threadWork.IsRunning()) {
        MQTT::Message message;
        int temperature = -1;
        int ret = MqttGetTemperature(temperature);
        if (ret == -1) {
            LOG_MM_E(MQTT_CLIENT, "MqttGetTemperature fail.");
        }
        
        const std::string interfaceName = "eth0";
        std::string ipAddress;
        ret = MqttGetIP(interfaceName, ipAddress);
        if (ret == -1) {
            LOG_MM_E(MQTT_CLIENT, "MqttGetIP fail.");
        }

        MemoryInfo memInfo = {0};
        ret = MqttMemoryInfo(memInfo);
        if (ret == -1) {
            LOG_MM_E(MQTT_CLIENT, "MqttGetIP fail.");
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
        LOG_MM_D(MQTT_CLIENT, "topic is %s", topic.c_str());

        if (rc != 0)
            LOG_MM_E(MQTT_CLIENT, "Error %d from sending QoS 0 message", rc);
        //must modify packet type
        client->yield(3*1000);
    }

    LOG_MM_E(MQTT_CLIENT, "---");
}
