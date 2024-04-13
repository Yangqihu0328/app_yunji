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
#include <string.h>
#include <map>
#include <mutex>
#include "AXSingleton.h"
#include "ax_skel_type.h"

#define MAX_DETECT_RESULT_COUNT (64)

typedef enum {
    DETECT_TYPE_UNKNOWN = 0,
    DETECT_TYPE_FACE = 1,
    DETECT_TYPE_BODY = 2,
    DETECT_TYPE_VEHICLE = 3,
    DETECT_TYPE_PLATE = 4,
    DETECT_TYPE_CYCLE = 5,
    DETECT_TYPE_BUTT
} DETECT_TYPE_E;

typedef struct {
    DETECT_TYPE_E eType;
    AX_U64 nTrackId;
    AX_SKEL_RECT_T tBox;
} DETECT_RESULT_ITEM_T;

typedef struct DETECT_RESULT_S {
    AX_U64 nSeqNum;
    AX_U32 nW;
    AX_U32 nH;
    AX_U32 nCount;
    AX_S32 nGrpId;
    DETECT_RESULT_ITEM_T item[MAX_DETECT_RESULT_COUNT];

    DETECT_RESULT_S(AX_VOID) {
        memset(this, 0, sizeof(*this));
    }

} DETECT_RESULT_T;

/**
 * @brief
 *
 */
class CDetectResult : public CAXSingleton<CDetectResult> {
    friend class CAXSingleton<CDetectResult>;

public:
    //这种都是要上锁，因为设计到多线程，因为在其他地方get的时候，就出现同时访问同个变量。
    //并且这里的m_mapRlts是公用的
    AX_BOOL Set(AX_S32 nGrp, const DETECT_RESULT_T& fhvp) {
        std::lock_guard<std::mutex> lck(m_mtx);
        //CDetectResult是公用的，因此才要加锁处理
        m_mapRlts[nGrp] = fhvp;

        //每个类型计数
        for (AX_U32 i = 0; i < fhvp.nCount; ++i) {
            ++m_arrCount[fhvp.item[i].eType];
        }

        return AX_TRUE;
    }

    AX_BOOL Get(AX_S32 nGrp, DETECT_RESULT_T& fhvp) {
        std::lock_guard<std::mutex> lck(m_mtx);
        if (m_mapRlts.end() == m_mapRlts.find(nGrp)) {
            return AX_FALSE;
        }

        fhvp = m_mapRlts[nGrp];
        return AX_TRUE;
    }

    AX_U64 GetTotalCount(DETECT_TYPE_E eType) {
        std::lock_guard<std::mutex> lck(m_mtx);
        return m_arrCount[eType];
    }

    AX_VOID Clear(AX_VOID) {
        std::lock_guard<std::mutex> lck(m_mtx);
        memset(m_arrCount, 0, sizeof(m_arrCount));
        m_mapRlts.clear();
    }

protected:
    CDetectResult(AX_VOID) noexcept = default;
    virtual ~CDetectResult(AX_VOID) = default;

private:
    std::mutex m_mtx;
    std::map<AX_S32, DETECT_RESULT_T> m_mapRlts;
    AX_U64 m_arrCount[DETECT_TYPE_BUTT] = {0};
};
