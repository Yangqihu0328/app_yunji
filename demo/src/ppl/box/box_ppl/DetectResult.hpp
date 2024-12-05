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
#include <iostream>
#include <set>
#include <vector>
#include <unordered_set>
#include "AXSingleton.h"
#include "ax_skel_type.h"

#define MAX_DETECT_RESULT_COUNT (32)
#define MAX_CHANNEL_SIZE (16)
#define MAX_RESULT_SIZE (5)

//临时写一个算法列表，火焰，动物，手势，抽烟
typedef enum {
    DETECT_TYPE_PEOPLE = 0,
    DETECT_TYPE_VEHICLE = 1,
    DETECT_TYPE_FACE = 2,
    DETECT_TYPE_FIRE = 3,
    DETECT_TYPE_TOTAL
} DETECT_TYPE_E;

typedef struct {
    DETECT_TYPE_E eType;
    AX_U64 nTrackId;
    AX_SKEL_RECT_T tBox;

    /*
    0到1之间的值，表示人脸质量，越高越好
    */
    AX_F32 quality;

    /*
    人体状态： 0：正面， 1：侧面，2：背面， 3：非人
    */
    AX_S32 status;

    /*
    车辆类型: 0：UNKNOWN 1：SEDAN 2：SUV 3：BUS 4：MICROBUS 5：TRUCK
    */
    AX_S32 cartype;
    /*
    如果 b_is_track_plate = 1，则表示当前帧没有识别到车牌，返回的是历史上 track_id 上一次识别到的车牌结果
    如果 b_is_track_plate = 0，且 len_plate_id > 0, 则表示当前帧识别到了车牌
    如果 b_is_track_plate = 0，且 len_plate_id = 0, 则表示当前帧没有识别到车牌，且是历史上 track_id 也没有结果
    */
    AX_S32 b_is_track_plate;
    AX_S32 len_plate_id;
    AX_S32 plate_id[16];

} DETECT_RESULT_ITEM_T;

typedef struct DETECT_RESULT_S {
    AX_U64 nSeqNum;
    AX_U32 nW;
    AX_U32 nH;
    AX_U32 nCount;
    AX_S32 nGrpId;
    AX_U32 nAlgoType;
    AX_BOOL result_diff;
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
    // 需要在这里进行组合
    AX_BOOL Set(AX_S32 nGrp, const DETECT_RESULT_T& result) {
        std::lock_guard<std::mutex> lck(m_mtx);

        DETECT_RESULT_T new_result;
        if (result.nAlgoType == DETECT_TYPE_E::DETECT_TYPE_PEOPLE) {
            new_result = HandleDetectPerson(nGrp, result);
        } else if (result.nAlgoType == DETECT_TYPE_E::DETECT_TYPE_VEHICLE) {
            new_result = HandleDetectVehicle(nGrp, result);
        } else if (result.nAlgoType == DETECT_TYPE_E::DETECT_TYPE_FACE) {
            new_result = HandleDetectFace(nGrp, result);
        } else if (result.nAlgoType == DETECT_TYPE_E::DETECT_TYPE_FIRE) {
            new_result = HandleDetectFire(nGrp, result);
        } else {
            new_result = result;
        }

        m_mapRlts[nGrp] = new_result;

        // 统计每个类型的总数量
        for (AX_U32 i = 0; i < new_result.nCount; ++i) {
            ++m_arrCount[new_result.nAlgoType];
        }

        return AX_TRUE;
    }

    AX_BOOL Get(AX_S32 nGrp, DETECT_RESULT_T& result) {
        std::lock_guard<std::mutex> lck(m_mtx);
        if (m_mapRlts.end() == m_mapRlts.find(nGrp)) {
            return AX_FALSE;
        }

        result = m_mapRlts[nGrp];
        return AX_TRUE;
    }

    DETECT_RESULT_T HandleDetectPerson(AX_S32 nGrp, const DETECT_RESULT_T& result) {
        DETECT_RESULT_T new_result = result;
        DETECT_RESULT_T last_result = m_mapRlts[nGrp];

        if (result.nCount != last_result.nCount) {
            new_result.result_diff = AX_TRUE;
        }

        return new_result;
    }

    DETECT_RESULT_T HandleDetectVehicle(AX_S32 nGrp, const DETECT_RESULT_T& result) {
        DETECT_RESULT_T new_result = result;
        DETECT_RESULT_T last_result = m_mapRlts[nGrp];

        if (result.nCount != last_result.nCount) {
            new_result.result_diff = AX_TRUE;
        }

        return new_result;
    }

    DETECT_RESULT_T HandleDetectFace(AX_S32 nGrp, const DETECT_RESULT_T& result) {
        DETECT_RESULT_T new_result = result;
        DETECT_RESULT_T last_result = m_mapRlts[nGrp];

        if (result.nCount != last_result.nCount) {
            new_result.result_diff = AX_TRUE;
        }

        return new_result;
    }

    DETECT_RESULT_T HandleDetectFire(AX_S32 nGrp, const DETECT_RESULT_T& result) {
        DETECT_RESULT_T new_result = result;
        DETECT_RESULT_T last_result = m_mapRlts[nGrp];

        if (result.nCount != last_result.nCount) {
            new_result.result_diff = AX_TRUE;
        } else {
            
        }

        return new_result;
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
    CDetectResult(AX_VOID)  = default;
    virtual ~CDetectResult(AX_VOID) = default;

private:
    std::mutex m_mtx;
    std::map<AX_S32, DETECT_RESULT_T> m_mapRlts;

    AX_U64 m_arrCount[DETECT_TYPE_TOTAL] = {0};
};
