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
#include "AiCardMstAppSys.hpp"
#include "AiCardMstConfig.hpp"
#include "AiSwitchConfig.hpp"
#include "Dispatcher.hpp"
#include "IObserver.h"
#include "IStreamHandler.hpp"
#include "VideoDecoder.hpp"
#include "Vo.hpp"
#include "TransferHelper.hpp"
#include "AiSwitchSimulator.hpp"
#include "VideoEncoder.h"
#include "VencObserver.h"
#include "AXRtspServer.h"
#include "AXRtspObserver.h"
#include "BaseLinkage.h"
#include "linker.hpp"

using namespace aicard_mst;

typedef struct _IPC_MOD_RELATIONSHIP_T {
    AX_U32 nIndex;
    IPC_MOD_INFO_T tSrcModChn;
    IPC_MOD_INFO_T tDstModChn;
    AX_BOOL bLink;

    _IPC_MOD_RELATIONSHIP_T() {
        memset(this, 0, sizeof(_IPC_MOD_RELATIONSHIP_T));
        tSrcModChn.eModType = E_PPL_MOD_TYPE_MAX;
        tDstModChn.eModType = E_PPL_MOD_TYPE_MAX;
    }

    AX_BOOL Valid() const {
        return (tSrcModChn.eModType != E_PPL_MOD_TYPE_MAX && tDstModChn.eModType != E_PPL_MOD_TYPE_MAX) ? AX_TRUE : AX_FALSE;
    }
} IPC_MOD_RELATIONSHIP_T, PPL_MOD_RELATIONSHIP_T;

class CAiCardMstBuilder final {
public:
    CAiCardMstBuilder(AX_VOID) noexcept = default;

    AX_BOOL Start(AX_VOID);
    AX_BOOL Stop(AX_VOID);

    AX_BOOL QueryStreamsAllEof(AX_VOID);

protected:
    AX_BOOL Init(AX_VOID);
    AX_BOOL DeInit(AX_VOID);

    AX_BOOL InitStreamer(const STREAM_CONFIG_T& streamConfig);
    AX_BOOL InitDisplay(const DISPVO_CONFIG_T& dispVoConfig);
    AX_BOOL InitEncoder(STREAM_CONFIG_T& streamConfig);
    AX_BOOL InitDispatcher(const std::string& strFontPath);
    AX_BOOL InitDecoder(const STREAM_CONFIG_T& streamConfig);
    AX_BOOL InitTransHelper();
    AX_BOOL InitAiSwitchSimlator();

protected:
    CAiCardMstAppSys m_sys;
    AX_U32 m_nDecodeGrpCount{0};
    std::unique_ptr<CVideoDecoder> m_vdec;
    std::unique_ptr<CVo> m_disp;
    std::vector<std::unique_ptr<CDispatcher>> m_arrDispatcher;
    IObserverUniquePtr m_dispObserver;
    std::vector<IObserverUniquePtr> m_arrDispatchObserver;
    std::vector<IStreamerHandlerPtr> m_arrStreamer;
    std::unique_ptr<CTransferHelper> m_transHelper;
    std::unique_ptr<CAiSwitchSimulator> m_aiSwitchSimulator;

    std::vector<CVideoEncoder*> m_vecVencInstance;
    std::vector<std::unique_ptr<IObserver>> m_vencObservers;
    std::vector<std::unique_ptr<IObserver>> m_vecRtspObs;

};