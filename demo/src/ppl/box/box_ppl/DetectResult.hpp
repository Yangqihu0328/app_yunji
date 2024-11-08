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

//临时写一个算法列表，火焰，动物，手势，抽烟
typedef enum {
    DETECT_TYPE_UNKNOWN = 0,
    DETECT_TYPE_FACE = 1,
    DETECT_TYPE_BODY = 2,
    DETECT_TYPE_VEHICLE = 3,
    DETECT_TYPE_PLATE = 4,
    DETECT_TYPE_CYCLE = 5,
    DETECT_TYPE_FIRE = 6,
    DETECT_TYPE_CAT = 7,
    DETECT_TYPE_DOG = 8,
    DETECT_TYPE_HAND_OK = 9,
    DETECT_TYPE_HAND_NO = 10,
    DETECT_TYPE_HAND_SMOKING = 11,
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
    AX_U32 nAlgoType;
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
    //需要在这里进行组合
    AX_BOOL Set(AX_S32 nGrp, const DETECT_RESULT_T& cur_result) {
        std::lock_guard<std::mutex> lck(m_mtx);

        auto &last_result = m_mapRlts[nGrp];
        DETECT_RESULT_T new_result, old_result;
        //说明是推理当前帧的多个算法
        if (last_result.nSeqNum == cur_result.nSeqNum && last_result.nAlgoType != cur_result.nAlgoType) {
            //先判断差异，找到最多的，从最多的增加。
            if (cur_result.nCount >= last_result.nCount) {
                new_result = cur_result;
                old_result = last_result;
            } else {
                new_result = last_result;
                old_result = cur_result;
            }

            int i = new_result.nCount, j = 0;
            int temp_count = new_result.nCount + last_result.nCount;
            int sum_count = temp_count < MAX_DETECT_RESULT_COUNT ? temp_count : MAX_DETECT_RESULT_COUNT;
            for (; i < sum_count; i++ && j++) {
                new_result.item[i].eType = old_result.item[j].eType;
                new_result.item[i].nTrackId  = old_result.item[j].nTrackId;
                new_result.item[i].tBox  = old_result.item[j].tBox;
            }
            new_result.nCount = sum_count;
        } else {
            new_result = cur_result;
        }

        m_mapRlts[nGrp] = new_result;

        for (AX_U32 i = 0; i < new_result.nCount; ++i) {
            ++m_arrCount[new_result.item[i].eType];
        }

        return AX_TRUE;
    }


    AX_BOOL Get(AX_S32 nGrp, DETECT_RESULT_T& result) {
        std::lock_guard<std::mutex> lck(m_mtx);
        if (m_mapRlts.end() == m_mapRlts.find(nGrp)) {
            return AX_FALSE;
        }

        result = m_mapRlts[nGrp];
        m_mapRlts.erase(nGrp);
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
