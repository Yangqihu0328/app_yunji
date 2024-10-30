/**************************************************************************************************
 *
 * Copyright (c) 2019-2023 Axera Semiconductor (Shanghai) Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor (Shanghai) Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor (Shanghai) Co., Ltd.
 *
 **************************************************************************************************/

#include "BoxConfig.hpp"
#include <unistd.h>
#include <sstream>
#include "GlobalDef.h"
#include "ax_global_type.h"
#include "ax_venc_rc.h"
using namespace std;
using namespace boxconf;

#ifdef __SLT__
#define BOX_RES_PATH "res/boxdemo/"
#else
#define BOX_RES_PATH ""
#endif

string CBoxConfig::GetExecPath(AX_VOID) {
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

AX_BOOL CBoxConfig::Init(AX_VOID) {
    string strIniPath = GetExecPath() + BOX_RES_PATH + "box.conf";
    if (!m_Ini.Load(strIniPath)) {
        return AX_FALSE;
    }

    return AX_TRUE;
}

STREAM_CONFIG_T CBoxConfig::GetStreamConfig(AX_VOID) {
    STREAM_CONFIG_T conf;
    const AX_CHAR *SECT = "STREAM";

    conf.nMaxGrpW = m_Ini.GetIntValue(SECT, "max width", 1920);
    conf.nMaxGrpH = m_Ini.GetIntValue(SECT, "max height", 1080);
    for (AX_U32 i = 0; i < 3; ++i) {
        AX_CHAR szKey[32];
        sprintf(szKey, "chn%d depth", i);
        conf.nChnDepth[i] = m_Ini.GetIntValue(SECT, szKey, 8);
    }
    conf.nDefaultFps = m_Ini.GetIntValue(SECT, "default fps", 0);
    conf.nInputMode = m_Ini.GetIntValue(SECT, "input mode", 0);

    conf.nUserPool = m_Ini.GetIntValue(SECT, "user pool", 1);
    if (conf.nUserPool > 2) {
        conf.nUserPool = 1;
    }

    conf.nMaxStreamBufSize = m_Ini.GetIntValue(SECT, "max stream buf size", 0x200000);
    AX_U32 nCount = m_Ini.GetIntValue(SECT, "count", 1);
    if (nCount > 0) {
        conf.v.resize(nCount);
        for (AX_U32 i = 1; i <= nCount; ++i) {
            AX_CHAR szKey[32];
            sprintf(szKey, "stream%02d", i);
            conf.v[i - 1] = m_Ini.GetStringValue(SECT, szKey, "");
        }
    }

    conf.nDecodeGrps = m_Ini.GetIntValue(SECT, "vdec count", 0);
    if (0 == conf.nDecodeGrps || conf.nDecodeGrps > nCount) {
        conf.nDecodeGrps = nCount;
    }

    /* SATA */
    conf.strSataPath = m_Ini.GetStringValue(SECT, "sata path", "");
    if (!conf.strSataPath.empty()) {
        if (conf.strSataPath[conf.strSataPath.length() - 1] != '/') {
            conf.strSataPath += '/';
        }
    }

    conf.nSataFileSize = m_Ini.GetIntValue(SECT, "max sata file size", 100);
    if (conf.nSataFileSize > 0) {
        conf.nSataFileSize *= (1024 * 1024);
    }

    conf.nMaxSpaceSize = m_Ini.GetIntValue(SECT, "max space size", 300);
    if (conf.nMaxSpaceSize > 0) {
        conf.nMaxSpaceSize *= (1024 * 1024);
    }

    conf.nLinkMode = m_Ini.GetIntValue(SECT, "linked mode", 0);

    return conf; /* RVO: optimized by compiler */
}

DETECT_CONFIG_T CBoxConfig::GetDetectConfig(AX_VOID) {
    DETECT_CONFIG_T conf;
    const AX_CHAR *SECT = "DETECT";

    conf.bEnable = (AX_BOOL)m_Ini.GetIntValue(SECT, "enable", 0);
    conf.nW = m_Ini.GetIntValue(SECT, "width", 960);
    conf.nH = m_Ini.GetIntValue(SECT, "height", 640);
    conf.nSkipRate = m_Ini.GetIntValue(SECT, "skip rate", 1);
    conf.nDepth = m_Ini.GetIntValue(SECT, "fifo depth", 1);
    conf.nVnpuMode = m_Ini.GetIntValue(SECT, "npu mode", 3);

    //使用的是stream的通道数量。
    int channel_num = AX_MIN(m_Ini.GetIntValue("STREAM", "count", 0), 32);

    for (AX_S32 i = 0; i < channel_num; ++i) {

        AX_CHAR str[32];
        sprintf(str, "channe_%02d", i);
        vector<AX_S32> vec;
        m_Ini.GetIntValue(SECT, str, vec);

        if (vec.size() == 5) {
            for(int j=0; j<3; j++) {
                conf.tChnParam[i].nPPL[j] = vec[j];
            }
            conf.tChnParam[i].bTrackEnable = (AX_BOOL)vec[3];
            if (conf.nVnpuMode == 0) {
                conf.tChnParam[i].nVNPU = 0;
            } else {
                conf.tChnParam[i].nVNPU = vec[4];
            }
        } else {
            for(int j=0; j<3; j++) {
                conf.tChnParam[i].nPPL[j] = 4;
            }
            conf.tChnParam[i].bTrackEnable = AX_FALSE;
            conf.tChnParam[i].nVNPU = 0;
        }
    }

    conf.strModelPath = m_Ini.GetStringValue(SECT, "model path", "");

    return conf; /* RVO: optimized by compiler */
}

MQTT_CONFIG_T CBoxConfig::GetMqttConfig(AX_VOID) {
    MQTT_CONFIG_T conf;
    const AX_CHAR *SECT = "MQTT";

    conf.topic = m_Ini.GetStringValue(SECT, "topic", "mqtt_msg");
    conf.hostname = m_Ini.GetStringValue(SECT, "hostname", "127.0.0.1");
    conf.name = m_Ini.GetStringValue(SECT, "name", "yunji");
    conf.passwd = m_Ini.GetStringValue(SECT, "passwd", "yunji123456");
    conf.version = m_Ini.GetIntValue(SECT, "version", 3);
    conf.port = m_Ini.GetIntValue(SECT, "port", 1883);

    return conf;
}

DISPVO_CONFIG_T CBoxConfig::GetDispVoConfig(const std::string &SECT) {
    DISPVO_CONFIG_T conf;
    // const AX_CHAR *SECT = "DISPC";

    conf.nDevId = m_Ini.GetIntValue(SECT, "dev", -1);
    conf.nHDMI = m_Ini.GetIntValue(SECT, "HDMI", 10);
    conf.nLayerDepth = m_Ini.GetIntValue(SECT, "layer depth", 3);
    /* if 0, vo using default tolerance, VO_LAYER_TOLERATION_DEF = 10*1000*1000 */
    conf.nTolerance = m_Ini.GetIntValue(SECT, "tolerance", 0);
    conf.bShowLogo = (AX_BOOL)m_Ini.GetIntValue(SECT, "show logo", 1);
    conf.bShowNoVideo = (AX_BOOL)m_Ini.GetIntValue(SECT, "show no video", 1);
    conf.strResDirPath = GetExecPath() + BOX_RES_PATH + "res";
    conf.strBmpPath = conf.strResDirPath + "/font.bmp";

    conf.bRecord = (AX_BOOL)m_Ini.GetIntValue(SECT, "record enable", 0);
    if (conf.bRecord && conf.nLayerDepth < 5) {
        /* VO -> VENC: increase more VB */
        conf.nLayerDepth = 5;
    }

    conf.strRecordPath = m_Ini.GetStringValue(SECT, "record path", "");
    conf.nMaxRecordSize = m_Ini.GetIntValue(SECT, "max record size", 1024);
    conf.bRecordMuxer = (AX_BOOL)m_Ini.GetIntValue(SECT, "record muxer", 1);

    conf.nDispType = (AX_BOOL)m_Ini.GetIntValue(SECT, "disp type", 2);

    conf.bOnlineMode = (AX_BOOL)m_Ini.GetIntValue(SECT, "online mode", 0);

    return conf; /* RVO: optimized by compiler */
}

VENC_CONFIG_T CBoxConfig::GetVencConfig(AX_VOID) {
    VENC_CONFIG_T conf;
    const AX_CHAR *SECT = "VENC";

    conf.nFifoDepth[0] = m_Ini.GetIntValue(SECT, "in fifo depth", 2);
    conf.nFifoDepth[1] = m_Ini.GetIntValue(SECT, "out fifo depth", 2);

    string strValue = m_Ini.GetStringValue(SECT, "encoder", "h264");
    if (string::npos != strValue.find("hevc")) {
        conf.nPayloadType = PT_H265;
    } else {
        conf.nPayloadType = PT_H264;
    }

    conf.nBitRate = m_Ini.GetIntValue(SECT, "bitrate", 8192);
    conf.nGop = m_Ini.GetIntValue(SECT, "gop", 0);
    strValue = m_Ini.GetStringValue(SECT, "rc type", "CBR");
    if (string::npos != strValue.find("AVBR")) {
        if (PT_H265 == conf.nPayloadType) {
            conf.nRCType = AX_VENC_RC_MODE_H265AVBR;
        } else {
            conf.nRCType = AX_VENC_RC_MODE_H264AVBR;
        }
    } else if (string::npos != strValue.find("VBR")) {
        if (PT_H265 == conf.nPayloadType) {
            conf.nRCType = AX_VENC_RC_MODE_H265VBR;
        } else {
            conf.nRCType = AX_VENC_RC_MODE_H264VBR;
        }
    } else {
        if (PT_H265 == conf.nPayloadType) {
            conf.nRCType = AX_VENC_RC_MODE_H265CBR;
        } else {
            conf.nRCType = AX_VENC_RC_MODE_H264CBR;
        }
    }
    conf.nMinQp = m_Ini.GetIntValue(SECT, "min qp", 0);
    conf.nMaxQp = m_Ini.GetIntValue(SECT, "max qp", 51);
    conf.nMinIQp = m_Ini.GetIntValue(SECT, "min iqp", 0);
    conf.nMaxIQp = m_Ini.GetIntValue(SECT, "max iqp", 51);
    conf.nMinIProp = m_Ini.GetIntValue(SECT, "min i prop", 10);
    conf.nMaxIProp = m_Ini.GetIntValue(SECT, "max i prop", 40);
    conf.nIntraQpDelta = m_Ini.GetIntValue(SECT, "intra qp delta", -2);

    return conf;
}

COMPRESS_CONFIG_T CBoxConfig::GetCompressConfig(AX_VOID) {
    COMPRESS_CONFIG_T conf;
    const AX_CHAR *SECT = "FBC";
    conf.nMode = m_Ini.GetIntValue(SECT, "compress mode", 0);
    if (conf.nMode >= AX_COMPRESS_MODE_BUTT) {
        conf.nMode = AX_COMPRESS_MODE_NONE;
    }

    conf.nLv = m_Ini.GetIntValue(SECT, "compress level", 0);

    return conf;
}

UT_CONFIG_T CBoxConfig::GetUTConfig(AX_VOID) {
    UT_CONFIG_T conf;
    const AX_CHAR *SECT = "UT";
    conf.nMaxSendNaluIntervalMilliseconds = m_Ini.GetIntValue(SECT, "PlaybackMaxSendNaluIntervalMilliseconds", 0);

    return conf;
}

AX_BOOL CBoxConfig::AddStreamUrl(AX_S32 channelId, std::string& channelUrl) {
    AX_BOOL bRet = AX_FALSE;
    const AX_CHAR *SECT = "STREAM";

    do {
        AX_CHAR szKey[32];
        sprintf(szKey, "stream%02d", channelId);
        printf("update stream id: %s to %s\n", szKey, channelUrl.c_str());
        m_Ini.SetStringValue(SECT, szKey, channelUrl);
    } while (0);

    return bRet;
}

AX_BOOL CBoxConfig::RemoveStreamUrl(AX_S32 channelId) {
    const AX_CHAR *SECT = "STREAM";

    do {
        AX_CHAR szKey[32];
        sprintf(szKey, "stream%02d", channelId);
        m_Ini.DeleteValue(SECT, szKey);
    } while (0);

    return AX_TRUE;
}

AX_BOOL CBoxConfig::AddAlgoTask(AX_S32 channelId, std::vector<int> &task_vec) {
    AX_BOOL bRet = AX_FALSE;
    const AX_CHAR *SECT = "DETECT";

    do {
        AX_CHAR szKey[32];
        sprintf(szKey, "channel%02d", channelId);

        std::ostringstream oss; // 用于构建字符串
        oss << "["; // 开始方括号

        // 将 vector 中的数字添加到字符串流
        for (size_t i = 0; i < task_vec.size(); ++i) {
            oss << task_vec[i];
            if (i < task_vec.size() - 1) {
                oss << ", "; // 添加逗号和空格
            }
        }

        // 默认为带跟踪算法和使用STD的NPU
        oss << ", 1, 0]"; // 结束方括号
        printf("update channel: %s to %s\n", szKey, oss.str().c_str());
        m_Ini.SetStringValue(SECT, szKey, oss.str());
    } while (0);

    return bRet;
}

AX_BOOL CBoxConfig::RemoveAlgoTask(AX_S32 channelId) {
    const AX_CHAR *SECT = "DETECT";

    do {
        AX_CHAR szKey[32];
        sprintf(szKey, "channel%02d", channelId);
        m_Ini.DeleteValue(SECT, szKey);
    } while (0);

    return AX_TRUE;
}

AX_BOOL CBoxConfig::GetStreamCount(AX_S32 &count) {
    const AX_CHAR *SECT = "STREAM";

    count = m_Ini.GetIntValue(SECT, "count", 0);

    return AX_TRUE;
}
