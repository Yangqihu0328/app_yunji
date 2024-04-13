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
#include <exception>
#include "AXFrame.hpp"
#include "IObserver.h"
#include "Vo.hpp"

/**
 * @brief
 *
 */
class CVoObserver final : public IObserver {
public:
    //原来传递的m_disp是作为这个的构造函数，第二个参数就是作为channel为1
    CVoObserver(CVo* pSink, AX_U32 nChn) noexcept : m_pSink(pSink), m_nChn(nChn){};
    virtual ~CVoObserver(AX_VOID) = default;

    //disp patch回调的是这个OnRecvData.相当于把数据发送出去，也就是disp和disp的channel要一样。
    AX_BOOL OnRecvData(OBS_TARGET_TYPE_E eTarget, AX_U32 nGrp, AX_U32 nChn, AX_VOID* pData) override {
        if (nullptr == pData) {
            return AX_FALSE;
        }
        //只有channel1有效
        if (m_nChn == nChn) {
            CAXFrame* pFrame = (CAXFrame*)pData;
            //这里就是vo把数据发送出去。这个也是channel1发送
            return m_pSink->SendFrame(m_pSink->GetVideoChn(nGrp), *pFrame, -1);
        } else {
            return AX_TRUE;
        }
    }

    AX_BOOL OnRegisterObserver(OBS_TARGET_TYPE_E eTarget, AX_U32 nGrp, AX_U32 nChn, OBS_TRANS_ATTR_PTR pParams) override {
        return AX_TRUE;
    }

private:
    CVo* m_pSink;
    AX_U32 m_nChn;
};