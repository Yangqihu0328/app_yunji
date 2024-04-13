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
    //相当于这里完成初始化
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
    //这个要使用默认为0，后面会经常考虑到
    conf.nDefaultFps = m_Ini.GetIntValue(SECT, "default fps", 0);
    //默认为frame
    conf.nInputMode = m_Ini.GetIntValue(SECT, "input mode", 0);
    //vdec使用私有pool
    conf.nUserPool = m_Ini.GetIntValue(SECT, "user pool", 1);
    if (conf.nUserPool > 2) {
        conf.nUserPool = 1;
    }

    conf.nMaxStreamBufSize = m_Ini.GetIntValue(SECT, "max stream buf size", 0x200000);
    //也就是下面的stream*填再多也没用，受限于count
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
    //存储路径
    conf.strSataPath = m_Ini.GetStringValue(SECT, "sata path", "");
    if (!conf.strSataPath.empty()) {
        if (conf.strSataPath[conf.strSataPath.length() - 1] != '/') {
            conf.strSataPath += '/';
        }
    }

    //设置文件大小M
    conf.nSataFileSize = m_Ini.GetIntValue(SECT, "max sata file size", 100);
    if (conf.nSataFileSize > 0) {
        conf.nSataFileSize *= (1024 * 1024);
    }

    //设置空间大小
    conf.nMaxSpaceSize = m_Ini.GetIntValue(SECT, "max space size", 300);
    if (conf.nMaxSpaceSize > 0) {
        conf.nMaxSpaceSize *= (1024 * 1024);
    }

    //vdec直接unlink vo,因为叠加rgn
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
    //每一路算法的fifo，这个buf不知道从哪来的，没见分配
    conf.nDepth = m_Ini.GetIntValue(SECT, "fifo depth", 1);
    conf.nVnpuMode = m_Ini.GetIntValue(SECT, "npu mode", 3);
    //这个地方只是取3，应该是要跟随grp num才对
    conf.nChannelNum = m_Ini.GetIntValue(SECT, "channel num", 1);
    conf.nChannelNum = AX_MIN(conf.nChannelNum, 32);

    //读取每一路channel的算法配置，暂时就两种算法，人车非和人脸并且可配置跟踪
    for (AX_S32 i = 0; i < conf.nChannelNum; ++i) {
        std::string str = "channel" + std::to_string(i) + " attr";

        vector<AX_S32> vec;
        m_Ini.GetIntValue(SECT, str, vec);

        if (vec.size() == 3) {
            conf.tChnParam[i].nPPL = vec[0];
            conf.tChnParam[i].bTrackEnable = (AX_BOOL)vec[1];
            if (conf.nVnpuMode == 0) {
                conf.tChnParam[i].nVNPU = 0;
            } else {
                conf.tChnParam[i].nVNPU = vec[2];
            }
        } else {
            conf.tChnParam[i].nPPL = 4;
            conf.tChnParam[i].bTrackEnable = AX_FALSE;
            conf.tChnParam[i].nVNPU = 0;
        }
    }

    conf.strModelPath = m_Ini.GetStringValue(SECT, "model path", "");

    return conf; /* RVO: optimized by compiler */
}

DISPVO_CONFIG_T CBoxConfig::GetDispVoConfig(const std::string &SECT) {
    DISPVO_CONFIG_T conf;
    // const AX_CHAR *SECT = "DISPC";

    conf.nDevId = m_Ini.GetIntValue(SECT, "dev", -1);
    //这个hdmi就是为了设置分辨率的宽高
    conf.nHDMI = m_Ini.GetIntValue(SECT, "HDMI", 10);
    //设置layaer的vb要求
    conf.nLayerDepth = m_Ini.GetIntValue(SECT, "layer depth", 3);
    /* if 0, vo using default tolerance, VO_LAYER_TOLERATION_DEF = 10*1000*1000 */
    conf.nTolerance = m_Ini.GetIntValue(SECT, "tolerance", 0);
    conf.bShowLogo = (AX_BOOL)m_Ini.GetIntValue(SECT, "show logo", 1);
    conf.bShowNoVideo = (AX_BOOL)m_Ini.GetIntValue(SECT, "show no video", 1);
    //可以配置显示log以及配置图片
    conf.strResDirPath = GetExecPath() + BOX_RES_PATH + "res";
    conf.strBmpPath = conf.strResDirPath + "/font.bmp";
    //配置record，对应了开启一个服务
    conf.bRecord = (AX_BOOL)m_Ini.GetIntValue(SECT, "record enable", 0);
    //记录的话，需要多两个vb?
    if (conf.bRecord && conf.nLayerDepth < 5) {
        /* VO -> VENC: increase more VB */
        conf.nLayerDepth = 5;
    }

    //这个地方是否与stream的stat配置类似？这个地方在确定一下
    conf.strRecordPath = m_Ini.GetStringValue(SECT, "record path", "");
    conf.nMaxRecordSize = m_Ini.GetIntValue(SECT, "max record size", 1024);
    conf.bRecordMuxer = (AX_BOOL)m_Ini.GetIntValue(SECT, "record muxer", 1);

    conf.nDispType = (AX_BOOL)m_Ini.GetIntValue(SECT, "disp type", 2);

    //默认同显，如果配置disp1才是异显
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

    //这个码率控制数据量
    conf.nBitRate = m_Ini.GetIntValue(SECT, "bitrate", 8192);
    //看看后面是不是等于fps
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