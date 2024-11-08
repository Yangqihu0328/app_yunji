 /**************************************************************************************************
 *
 * Copyright (c) 2019-2023 Axera Semiconductor (Shanghai) Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor (Shanghai) Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor (Shanghai) Co., Ltd.
 *
 **************************************************************************************************/

#pragma once
#include <memory>
#include <string>
#include <vector>
#include "BoxAppSys.hpp"
#include "BoxConfig.hpp"
#include "BoxRecorder.hpp"
#include "Detector.hpp"
#include "Dispatcher.hpp"
#include "IObserver.h"
#include "IStreamHandler.hpp"
#include "StreamRecorder.hpp"
#include "EncoderOptionHelper.h"
#include "VideoDecoder.hpp"
#include "VideoEncoder.h"
#include "VencObserver.h"
#include "AXRtspServer.h"
#include "AXRtspObserver.h"
#include "Vo.hpp"
#include "linker.hpp"

//3rd
#include "MqttClient.hpp"
using namespace boxconf;

enum class AX_DISPDEV_TYPE {
    PRIMARY = 0,
    SECONDARY = 1,
};
class CBoxBuilder : public CAXSingleton<CBoxBuilder> {
    friend class CAXSingleton<CBoxBuilder>;

public:
    CBoxBuilder(AX_VOID) noexcept = default;

    AX_BOOL Start(AX_VOID);
    AX_BOOL StopAllStreams(AX_VOID);
    AX_BOOL StartStream(AX_S32 channelId);
    AX_BOOL StopStream(AX_S32 channelId);
    AX_BOOL AddStream(AX_S32 channelId);
    AX_BOOL RemoveStream(AX_S32 channelId);
    AX_BOOL WaitDone(AX_VOID);

    AX_BOOL QueryStreamsAllEof(AX_VOID);

protected:
    AX_BOOL Init(AX_VOID);
    AX_BOOL DeInit(AX_VOID);

    AX_BOOL InitStreamer(const STREAM_CONFIG_T& streamConfig);
    AX_BOOL InitDisplay(AX_DISPDEV_TYPE enDispDev, const DISPVO_CONFIG_T& dispVoConfig, AX_U32 nChnCount);

    AX_BOOL InitDispRecorder(const std::string& strRecordPath, AX_S32 nMaxRecordSize, AX_BOOL bMP4);

    AX_BOOL InitDispatcher(const string& strFontPath, AX_U32 nDispType);
    AX_BOOL InitEncoder(STREAM_CONFIG_T& streamConfig);
    AX_BOOL InitMqtt();
    AX_BOOL InitJenc();

    AX_BOOL InitDetector(const DETECT_CONFIG_T& detectConfig);
    AX_BOOL InitDecoder(const STREAM_CONFIG_T& streamConfig);
    AX_BOOL CheckDiskSpace(const STREAM_CONFIG_T& streamConfig);

#if defined(__RECORD_VB_TIMESTAMP__)
    AX_VOID AllocTimestampBufs(AX_VOID);
    AX_VOID FreeTimestampBufs(AX_VOID);
#endif

protected:
    CBoxAppSys m_sys;
    AX_U32 m_nDecodeGrpCount{0};
    std::unique_ptr<CVideoDecoder> m_vdec;
    std::unique_ptr<CDetector> m_detect;
    IObserverUniquePtr m_detectObserver;
    std::vector<IStreamerHandlerPtr> m_arrStreamer;
    std::vector<std::unique_ptr<CStreamRecorder>> m_sataWritter;
    std::unique_ptr<CBoxRecorder> m_dispRecorder;

    std::vector<std::unique_ptr<CDispatcher>> m_arrDispatcher;
    std::vector<IObserverUniquePtr> m_arrDispatchObserver;
    std::unique_ptr<MqttClient> mqtt_client;
    std::unique_ptr<CJpegEncoder> m_jenc;
    std::vector<CVideoEncoder*> m_vecVencInstance;
    std::vector<std::unique_ptr<IObserver>> m_vencObservers;
    std::vector<std::unique_ptr<IObserver>> m_vecRtspObs;

    // primary
    std::unique_ptr<CVo> m_disp;
    IObserverUniquePtr m_dispObserver;

    // secondary
    std::unique_ptr<CVo> m_dispSecondary;
    IObserverUniquePtr m_dispObserverSecondary;
    

    CLinker m_linker;

#if defined(__RECORD_VB_TIMESTAMP__)
    std::vector<AX_MOD_INFO_T> m_arrTimestampMods;
#endif
};
