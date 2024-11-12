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
#include <unordered_set>
#include "AXSingleton.h"
#include "ax_skel_type.h"

#define MAX_DETECT_RESULT_COUNT (64)
#define MAX_CHANNEL_SIZE (16)
#define MAX_RESULT_SIZE (5)

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
    AX_U32 result_diff;
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

        DETECT_RESULT_T new_result, few_result;
        //说明是推理当前帧的多个算法
        if (last_result.nSeqNum == cur_result.nSeqNum && last_result.nAlgoType != cur_result.nAlgoType) {
            //先判断差异，找到最多的，从最多的增加。
            if (cur_result.nCount >= last_result.nCount) {
                new_result = cur_result;
                few_result = last_result;
            } else {
                new_result = last_result;
                few_result = cur_result;
                new_result.nAlgoType = cur_result.nAlgoType;
            }

            int i = new_result.nCount, j = 0;
            int temp_count = new_result.nCount + last_result.nCount;
            int sum_count = temp_count < MAX_DETECT_RESULT_COUNT ? temp_count : MAX_DETECT_RESULT_COUNT;
            for (; i < sum_count; i++ && j++) {
                new_result.item[i].eType = few_result.item[j].eType;
                new_result.item[i].nTrackId  = few_result.item[j].nTrackId;
                new_result.item[i].tBox  = few_result.item[j].tBox;
            }
            new_result.nCount = sum_count;
        } else {
            new_result = cur_result;
        }

        auto track_id_set = [](const DETECT_RESULT_T& result) {
            std::unordered_set<int> track_ids;
            for (int i = 0; i < result.nCount; ++i) {
                track_ids.insert(result.item[i].nTrackId);
            }
            return track_ids;
        };

        // 比较两个track_id集合是否存在差异
        auto has_difference = [](const std::unordered_set<int>& set1, const DETECT_RESULT_T& result) {
            for (int j = 0; j < result.nCount; ++j) {
                if (set1.find(result.item[j].nTrackId) == set1.end()) {
                    return true; // 找到不同的track_id
                }
            }
            return false; // 所有track_id都匹配
        };

        //现在的问题：检测容易漏检，导致跟踪算法容易跟丢,容易出现新的track id
        //如果某一帧跟丢的话，判断上一帧的结果，下一帧肯定找不到上一帧的track id
        //两次的数量相同，说明当前是较稳定的,把这个结果保存起来。
        if (last_result.nCount == new_result.nCount) {
            //这里的result就是表示上一次稳定目标的结果
            auto result = channel_result[nGrp];

            if (result.nCount == 0) {
                new_result.result_diff = true;
            } else {
                std::unordered_set<int> last_track_ids = track_id_set(result);
                //当前结果与上一次稳定结果相比较
                new_result.result_diff = has_difference(last_track_ids, new_result);
            }

            if (new_result.result_diff == true) {
                channel_result[nGrp] = new_result;
            }
        }

        m_mapRlts[nGrp] = new_result;

        //统计每个类型的总数量
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
    CDetectResult(AX_VOID) noexcept : channel_result(16){};
    virtual ~CDetectResult(AX_VOID) = default;

private:
    std::mutex m_mtx;
    std::map<AX_S32, DETECT_RESULT_T> m_mapRlts;
    std::vector<DETECT_RESULT_T> channel_result;

    AX_U64 m_arrCount[DETECT_TYPE_BUTT] = {0};
};
