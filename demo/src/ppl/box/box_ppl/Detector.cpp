/**************************************************************************************************
 *
 * Copyright (c) 2019-2023 Axera Semiconductor (Shanghai) Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor (Shanghai) Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor (Shanghai) Co., Ltd.
 *
 **************************************************************************************************/

#include "Detector.hpp"
#include <string.h>
#include <chrono>
#include <unordered_set>
#include <exception>
#include "AXException.hpp"
#include "AppLogApi.h"
#include "DetectResult.hpp"
#if defined(__RECORD_VB_TIMESTAMP__)
#include "TimestampHelper.hpp"
#endif

using namespace std;
#define DETECTOR "SKEL"

#ifdef __USE_AX_ALGO
std::map<int, std::string> DetModelMap = {
    {AX_PPL_PEOPLE, "20241113.model"},
};

AX_VOID CDetector::RunDetect(AX_VOID *pArg) {
    LOG_M_C(DETECTOR, "detect thread is running");

    AX_U64 nFrameId = 0;
    AX_U32 nCurrGrp = 0;
    AX_U32 nNextGrp = 0;
    const AX_U32 TOTAL_GRP_COUNT = m_stAttr.nChannelNum;
    CAXFrame axFrame;
    AX_U32 nSkipCount = 0;
    while (m_DetectThread.IsRunning()) {
        for (nCurrGrp = nNextGrp; nCurrGrp < TOTAL_GRP_COUNT; ++nCurrGrp) {
            if (m_arrFrameQ[nCurrGrp].Pop(axFrame, 0)) {
                nSkipCount = 0;
                break;
            }

            if (++nSkipCount == TOTAL_GRP_COUNT) {
                this_thread::sleep_for(chrono::microseconds(1000));
                nSkipCount = 0;
            }
        }

        if (nCurrGrp == TOTAL_GRP_COUNT) {
            nNextGrp = 0;
            continue;
        } else {
            nNextGrp = nCurrGrp + 1;
            if (nNextGrp == TOTAL_GRP_COUNT) {
                nNextGrp = 0;
            }
        }

        LOG_M_D(DETECTOR, "runskel vdGrp %d vdChn %d frame %lld pts %lld phy 0x%llx %dx%d stride %d blkId 0x%x",
                axFrame.nGrp, axFrame.nChn,
                axFrame.stFrame.stVFrame.stVFrame.u64SeqNum, axFrame.stFrame.stVFrame.stVFrame.u64PTS,
                axFrame.stFrame.stVFrame.stVFrame.u64PhyAddr[0], axFrame.stFrame.stVFrame.stVFrame.u32Width,
                axFrame.stFrame.stVFrame.stVFrame.u32Height, axFrame.stFrame.stVFrame.stVFrame.u32PicStride[0],
                axFrame.stFrame.stVFrame.stVFrame.u32BlkId[0]);

        auto &flag_vec = d_vec[nCurrGrp];
        //遍历三种算法是否使能
        for (int index = 0; index < 1; index++) {
            if (flag_vec[index]) {
                ax_image_t ax_image;
                auto frame_info = axFrame.stFrame.stVFrame.stVFrame;
                #if 0
                // ax_image.pPhy = (unsigned long long int)frame_info.u64PhyAddr;
                // ax_image.pVir = (void *)frame_info.u64VirAddr[0];
                // ax_image.nSize = frame_info.u32FrameSize;
                // ax_image.nWidth = frame_info.u32Width;
                // ax_image.nHeight = frame_info.u32Height;
                // ax_image.eDtype = ax_color_space_nv12;
                // ax_image.tStride_W = frame_info.u32PicStride[0];
                #else
                if (0 == axFrame.stFrame.stVFrame.stVFrame.u64VirAddr[0]) {
                    axFrame.stFrame.stVFrame.stVFrame.u64VirAddr[0] =
                    (AX_U64)AX_POOL_GetBlockVirAddr(axFrame.stFrame.stVFrame.stVFrame.u32BlkId[0]);
                }
                int ret = ax_create_image(frame_info.u32Width, frame_info.u32Height, frame_info.u32PicStride[0],
                ax_color_space_nv12, &ax_image);
                if (ret != 0) {
                    LOG_M_E(DETECTOR, "alloc image fail\n");
                    break;
                }
                memcpy(ax_image.pVir, (void *)axFrame.stFrame.stVFrame.stVFrame.u64VirAddr[0], ax_image.nSize);
                #endif

                #ifdef __BOX_DEBUG__
                ofstream ofs;
                AX_CHAR szFile[64];
                sprintf(szFile, "./ai_dump_%d_%lld.yuv", axFrame.stFrame.stVFrame.stVFrame.u32FrameSize,
                axFrame.stFrame.stVFrame.stVFrame.u64SeqNum);
                ofs.open(szFile, ofstream::out | ofstream::binary | ofstream::trunc);
                if (0 == axFrame.stFrame.stVFrame.stVFrame.u64VirAddr[0]) {
                    axFrame.stFrame.stVFrame.stVFrame.u64VirAddr[0] =
                    (AX_U64)AX_POOL_GetBlockVirAddr(axFrame.stFrame.stVFrame.stVFrame.u32BlkId[0]);
                }
                ofs.write((const char*)axFrame.stFrame.stVFrame.stVFrame.u64VirAddr[0],
                                    axFrame.stFrame.stVFrame.stVFrame.u32FrameSize);
                ofs.close();
                #endif

                ax_result_t forward_result;
                ax_algorithm_inference(handle[nCurrGrp][index], &ax_image, &forward_result);
                ax_release_image(&ax_image);

                DETECT_RESULT_T result;
                result.nW = frame_info.u32Width;
                result.nH = frame_info.u32Height;
                result.nSeqNum = frame_info.u64SeqNum;
                result.nGrpId = axFrame.nGrp;
                result.nAlgoType = m_stAttr.tChnAttr[axFrame.nGrp].nPPL[index];
                result.nCount = forward_result.n_objects;

                for (AX_U32 i = 0; i < result.nCount; ++i) {
                    const auto& obj = forward_result.objects[i];
                    auto& item = result.item[i];
                    auto& tBox = item.tBox;

                    item.eType = static_cast<DETECT_TYPE_E>(obj.label);
                    item.nTrackId = obj.track_id;

                    // 提取边界框并校验宽度和高度
                    float x = obj.bbox.x;
                    float y = obj.bbox.y;
                    float w = obj.bbox.w;
                    float h = obj.bbox.h;

                    if (x + w + 1 > frame_info.u32Width) {
                        w = frame_info.u32Width - x - 1;
                    }

                    if (y + h + 1 > frame_info.u32Height) {
                        h = frame_info.u32Height - y - 1;
                    }

                    tBox.fX = x;
                    tBox.fY = y;
                    tBox.fW = std::max(0.0f, w);
                    tBox.fH = std::max(0.0f, h); // 确保宽高不会为负
                }

                if (result.nCount > 0) {
                    CDetectResult::GetInstance()->Set(axFrame.nGrp, result);
                }
            }
        }
        /* release frame after done */
        axFrame.DecRef();
    }

    LOG_M_D(DETECTOR, "%s: ---", __func__);
}

AX_BOOL CDetector::Init(const DETECTOR_ATTR_T &stAttr) {
    LOG_M_D(DETECTOR, "%s: +++", __func__);

    if (0 == stAttr.nChannelNum) {
        LOG_M_E(DETECTOR, "%s: 0 nChannelNum", __func__);
        return AX_FALSE;
    }

    m_stAttr = stAttr;

    if (m_stAttr.nSkipRate <= 0) {
        m_stAttr.nSkipRate = 1;
    }

    //都是预分配16个
    m_arrFrameQ = new (nothrow) CAXLockQ<CAXFrame>[stAttr.nChannelNum];
    if (!m_arrFrameQ) {
        LOG_M_E(DETECTOR, "%s: alloc queue fail", __func__);
        return AX_FALSE;
    } else {
        for (AX_U32 i = 0; i < stAttr.nChannelNum; ++i) {
            m_arrFrameQ[i].SetCapacity(-1);
        }
    }

    d_vec.resize(stAttr.nChannelNum);
    for (AX_U32 nChn = 0; nChn < m_stAttr.nChannelNum; ++nChn) {
        if (m_stAttr.tChnAttr[nChn].disable == 1) {
            continue;
        }

        for (AX_U32 algo_id = 0; algo_id < ALGO_MAX_NUM; ++algo_id) {
            ax_algorithm_init_t init_info;
            AX_U32 algo_config  = m_stAttr.tChnAttr[nChn].nPPL[algo_id];
            auto it = DetModelMap.find(algo_config);
            if (it == DetModelMap.end()) {
                d_vec[nChn][algo_id] = false;
                LOG_M_E(DETECTOR, "%s: Key not found in DetModelMap", __func__);
                break;
            }

            sprintf(init_info.model_file, DetModelMap[algo_config].c_str());
            if (ax_algorithm_init(&init_info, &handle[nChn][algo_id]) != 0) {
                LOG_M_E(DETECTOR, "%s: ax_algorithm_init fail", __func__);
                d_vec[nChn][algo_id] = false;
                break;
            }
            d_vec[nChn][algo_id] = true;
        }
    }

    LOG_M_D(DETECTOR, "%s: ---", __func__);
    return AX_TRUE;
}

AX_BOOL CDetector::DeInit(AX_VOID) {
    LOG_M_D(DETECTOR, "%s: +++", __func__);

    AX_S32 ret;
    if (m_DetectThread.IsRunning()) {
        LOG_M_E(DETECTOR, "%s: detect thread is running", __func__);
        return AX_FALSE;
    }

    for (AX_U32 nChn = 0; nChn < m_stAttr.nChannelNum; ++nChn) {
        for (AX_U32 algo_id = 0; algo_id < ALGO_MAX_NUM; ++algo_id) {
            if (d_vec[nChn][algo_id] == true && handle[nChn][algo_id] != nullptr) {
                ax_algorithm_deinit(handle[nChn][algo_id]);
            }
        }
    }

    if (m_arrFrameQ) {
        delete[] m_arrFrameQ;
        m_arrFrameQ = nullptr;
    }

    LOG_M_D(DETECTOR, "%s: ---", __func__);
    return AX_TRUE;
}

AX_BOOL CDetector::Start(AX_VOID) {
    LOG_M_D(DETECTOR, "%s: +++", __func__);

    if (!m_DetectThread.Start([this](AX_VOID *pArg) -> AX_VOID { RunDetect(pArg); }, this, "AppDetect", SCHED_FIFO, 99)) {
        LOG_M_E(DETECTOR, "%s: create detect thread fail", __func__);
        return AX_FALSE;
    }
    LOG_M_D(DETECTOR, "%s: ---", __func__);
    return AX_TRUE;
}

//指定id开始，实际上就是对m_arrFrameQ进行初始化和调用npu接口
AX_BOOL CDetector::StartId(int id, DETECTOR_CHN_ATTR_T det_attr) {
    LOG_M_W(DETECTOR, "%s: +++", __func__);
    do {
        auto &algo_type_arr = det_attr.nPPL;
        bool has_duplicate = false;
        std::unordered_set<int> seen;

        if (det_attr.disable == 1) {
            LOG_M_E(DETECTOR, "%s: CDetector disable", __func__);
        }

        for (AX_U32 algo_id = 0; algo_id < ALGO_MAX_NUM; ++algo_id) {
            if (seen.find(algo_type_arr[algo_id]) != seen.end()) {
                has_duplicate = true;
                break;
            }
            seen.insert(algo_type_arr[algo_id]);

            if (has_duplicate) {
                continue;
            }

            ax_algorithm_init_t init_info;
            AX_U32 algo_config  = algo_type_arr[algo_id];
            auto it = DetModelMap.find(algo_config);
            if (it == DetModelMap.end()) {
                d_vec[id][algo_id] = false;
                LOG_M_E(DETECTOR, "%s: Key not found in DetModelMap", __func__);
                break;
            }

            sprintf(init_info.model_file, DetModelMap[algo_config].c_str());
            if (ax_algorithm_init(&init_info, &handle[id][algo_id]) != 0) {
                LOG_M_E(DETECTOR, "%s: ax_algorithm_init fail", __func__);
                d_vec[id][algo_id] = false;
                break;
            }

            d_vec[id][algo_id] = true;
        }
    } while (0);

    LOG_M_D(DETECTOR, "%s: ---", __func__);
    return AX_TRUE;
}

AX_BOOL CDetector::Stop(AX_VOID) {
    LOG_M_D(DETECTOR, "%s: +++", __func__);

    m_DetectThread.Stop();

    if (m_arrFrameQ) {
        for (AX_U32 i = 0; i < m_stAttr.nChannelNum; ++i) {
            m_arrFrameQ[i].Wakeup();
        }
    }

    m_DetectThread.Join();

    if (m_arrFrameQ) {
        for (AX_U32 i = 0; i < m_stAttr.nChannelNum; ++i) {
            ClearQueue(i);
        }
    }

    LOG_M_D(DETECTOR, "%s: ---", __func__);
    return AX_TRUE;
}

//指定id停止，但是不会停止detector主线程。
AX_BOOL CDetector::StopId(int id) {
    LOG_M_D(DETECTOR, "%s: +++", __func__);

    //先把消息队列清空，再删除skel
    if (m_arrFrameQ) {
        m_arrFrameQ[id].Wakeup();
        ClearQueue(id);
    }

    if (handle[id]) {
        for (AX_U32 algo_id = 0; algo_id < ALGO_MAX_NUM; ++algo_id) {
            if (d_vec[id][algo_id] == true && handle[id][algo_id] != nullptr) {
                ax_algorithm_deinit(handle[id][algo_id]);
            }
        }
    }

    LOG_M_D(DETECTOR, "%s: ---", __func__);
    return AX_TRUE;
}

AX_BOOL CDetector::SendFrame(const CAXFrame &axFrame) {
    LOG_MM_I(DETECTOR, "recvfrm vdGrp %d vdChn %d frame %lld pts %lld phy 0x%llx %dx%d stride %d blkId 0x%x\n", axFrame.nGrp, axFrame.nChn,
            axFrame.stFrame.stVFrame.stVFrame.u64SeqNum, axFrame.stFrame.stVFrame.stVFrame.u64PTS,
            axFrame.stFrame.stVFrame.stVFrame.u64PhyAddr[0], axFrame.stFrame.stVFrame.stVFrame.u32Width,
            axFrame.stFrame.stVFrame.stVFrame.u32Height, axFrame.stFrame.stVFrame.stVFrame.u32PicStride[0],
            axFrame.stFrame.stVFrame.stVFrame.u32BlkId[0]);
    if (SkipFrame(axFrame)) {
        LOG_MM_I(DETECTOR, "dropfrm vdGrp %d vdChn %d frame %lld pts %lld phy 0x%llx %dx%d stride %d blkId 0x%x\n", axFrame.nGrp, axFrame.nChn,
                axFrame.stFrame.stVFrame.stVFrame.u64SeqNum, axFrame.stFrame.stVFrame.stVFrame.u64PTS,
                axFrame.stFrame.stVFrame.stVFrame.u64PhyAddr[0], axFrame.stFrame.stVFrame.stVFrame.u32Width,
                axFrame.stFrame.stVFrame.stVFrame.u32Height, axFrame.stFrame.stVFrame.stVFrame.u32PicStride[0],
                axFrame.stFrame.stVFrame.stVFrame.u32BlkId[0]);
        return AX_TRUE;
    }
    axFrame.IncRef();

#if defined(__RECORD_VB_TIMESTAMP__)
    CTimestampHelper::RecordTimestamp(axFrame.stFrame.stVFrame.stVFrame, axFrame.nGrp, axFrame.nChn, TIMESTAMP_SKEL_PUSH_FIFO);
#endif

    if (!m_arrFrameQ[axFrame.nGrp].Push(axFrame)) {
        LOG_M_E(DETECTOR, "%s: push frame %lld to q fail", __func__, axFrame.stFrame.stVFrame.stVFrame.u64SeqNum);
        axFrame.DecRef();
    }

    return AX_TRUE;
}

AX_BOOL CDetector::SkipFrame(const CAXFrame &axFrame) {
#ifdef __VDEC_PP_FRAME_CTRL__
    return AX_FALSE;
#else
    if (m_stAttr.nSkipRate <= 1) {
        return AX_FALSE;
    }

    if (axFrame.stFrame.stVFrame.stVFrame.u64SeqNum < 3) {
        return AX_TRUE;
    }

    return (1 == (axFrame.stFrame.stVFrame.stVFrame.u64SeqNum % m_stAttr.nSkipRate)) ? AX_FALSE : AX_TRUE;
#endif
}


AX_BOOL CDetector::Clear(AX_VOID) {
    if (m_arrFrameQ) {
        for (AX_U32 i = 0; i < m_stAttr.nChannelNum; ++i) {
            ClearQueue(i);
        }
    }

    return AX_TRUE;
}

AX_VOID CDetector::ClearQueue(AX_S32 nGrp) {
    AX_U32 nCount = m_arrFrameQ[nGrp].GetCount();
    if (nCount > 0) {
        CAXFrame axFrame;
        for (AX_U32 i = 0; i < nCount; ++i) {
            if (m_arrFrameQ[nGrp].Pop(axFrame, 0)) {
                axFrame.DecRef();
            }
        }
    }
}
#else
//同一帧被多个算法推理是正常情况。那么推理的结果应该都是存在同一个结果里面。
//考虑的问题，检测的框要一直存在，但是检测结果不要一直送。
static AX_VOID SkelResultCallback(AX_SKEL_HANDLE pHandle, AX_SKEL_RESULT_T *pstResult, AX_VOID *pUserData) {
    CDetector *pThis = (CDetector *)pUserData;
    if (!pThis) {
        THROW_AX_EXCEPTION("skel handle %p result callback user data is nil", pHandle);
    }

    SKEL_FRAME_PRIVATE_DATA_T *pPrivData = (SKEL_FRAME_PRIVATE_DATA_T *)(pstResult->pUserData);
    if (!pPrivData) {
        THROW_AX_EXCEPTION("skel handle %p frame private data is nil", pHandle);
    }

    //不对喔，每次fhvp会重新更新。而且你也会遇到路的算法推理结果
    DETECT_RESULT_T fhvp;
    fhvp.nW = pstResult->nOriginalWidth;
    fhvp.nH = pstResult->nOriginalHeight;
    fhvp.nSeqNum = pPrivData->nSeqNum;
    fhvp.nGrpId = pPrivData->nGrpId;
    fhvp.nAlgoType = pPrivData->nAlgoType;
    fhvp.nCount = pstResult->nObjectSize;

    AX_U32 index = 0;
    for (AX_U32 i = 0; i < fhvp.nCount && index < MAX_DETECT_RESULT_COUNT; ++i) {
        if (pstResult->pstObjectItems[i].eTrackState != AX_SKEL_TRACK_STATUS_NEW &&
            pstResult->pstObjectItems[i].eTrackState != AX_SKEL_TRACK_STATUS_UPDATE) {
            continue;
        }

        //相当于用item去保存数据，那么就对item进行维护即可，直接再item后面增加即可。
        if (0 == strcmp(pstResult->pstObjectItems[i].pstrObjectCategory, "body")) {
            fhvp.item[index].eType = DETECT_TYPE_BODY;
        } else if (0 == strcmp(pstResult->pstObjectItems[i].pstrObjectCategory, "vehicle")) {
            fhvp.item[index].eType = DETECT_TYPE_VEHICLE;
        } else if (0 == strcmp(pstResult->pstObjectItems[i].pstrObjectCategory, "cycle")) {
            fhvp.item[index].eType = DETECT_TYPE_CYCLE;
        } else if (0 == strcmp(pstResult->pstObjectItems[i].pstrObjectCategory, "face")) {
            fhvp.item[index].eType = DETECT_TYPE_FACE;
        } else if (0 == strcmp(pstResult->pstObjectItems[i].pstrObjectCategory, "plate")) {
            fhvp.item[index].eType = DETECT_TYPE_PLATE;
        } else if (0 == strcmp(pstResult->pstObjectItems[i].pstrObjectCategory, "fire")) {
            fhvp.item[index].eType = DETECT_TYPE_FIRE;
        } else if (0 == strcmp(pstResult->pstObjectItems[i].pstrObjectCategory, "cat")) {
            fhvp.item[index].eType = DETECT_TYPE_CAT;
        } else {
            LOG_M_W(DETECTOR, "unknown detect result %s of vdGrp %d frame %lld (skel %lld)",
                    pstResult->pstObjectItems[i].pstrObjectCategory, fhvp.nGrpId, fhvp.nSeqNum, pstResult->nFrameId);
            fhvp.item[index].eType = DETECT_TYPE_UNKNOWN;
        }
        
        fhvp.item[index].nTrackId = pstResult->pstObjectItems[i].nTrackId;
        fhvp.item[index].tBox = pstResult->pstObjectItems[i].stRect;
        index++;
    }

    fhvp.nCount = index;

    /* save fhvp result */
    //有结果才存储
    if (fhvp.nCount > 0) {
        CDetectResult::GetInstance()->Set(pPrivData->nGrpId, fhvp);
    }

    /* release fhvp result */
    AX_SKEL_Release((AX_VOID *)pstResult);

    /* giveback private data */
    pThis->ReleaseSkelPrivateData(pPrivData);
}

AX_VOID CDetector::RunDetect(AX_VOID *pArg) {
    LOG_M_C(DETECTOR, "detect thread is running");

    AX_U64 nFrameId = 0;
    AX_U32 nCurrGrp = 0;
    AX_U32 nNextGrp = 0;
    const AX_U32 TOTAL_GRP_COUNT = m_stAttr.nGrpCount;
    CAXFrame axFrame;
    AX_U32 nSkipCount = 0;
    while (m_DetectThread.IsRunning()) {
        //保留16个frame队列，只是增加遍历次数
        for (nCurrGrp = nNextGrp; nCurrGrp < TOTAL_GRP_COUNT; ++nCurrGrp) {
            if (m_arrFrameQ[nCurrGrp].Pop(axFrame, 0)) {
                nSkipCount = 0;
                break;
            }

            if (++nSkipCount == TOTAL_GRP_COUNT) {
                this_thread::sleep_for(chrono::microseconds(1000));
                nSkipCount = 0;
            }
        }

        if (nCurrGrp == TOTAL_GRP_COUNT) {
            nNextGrp = 0;
            continue;
        } else {
            nNextGrp = nCurrGrp + 1;
            if (nNextGrp == TOTAL_GRP_COUNT) {
                nNextGrp = 0;
            }
        }

        SKEL_FRAME_PRIVATE_DATA_T *pPrivData = m_skelData.borrow();
        if (!pPrivData) {
            LOG_M_E(DETECTOR, "%s: borrow skel frame private data fail", __func__);
            axFrame.DecRef();
            continue;
        } else {
            pPrivData->nSeqNum = axFrame.stFrame.stVFrame.stVFrame.u64SeqNum;
            pPrivData->nGrpId = axFrame.nGrp;
            pPrivData->nSkelChn = axFrame.nGrp % m_stAttr.nChannelNum;
        }

        AX_SKEL_FRAME_T skelFrame;
        skelFrame.nFrameId = ++nFrameId; /* skel recommends to unique frame id */
        skelFrame.nStreamId = axFrame.nGrp;
        skelFrame.stFrame = axFrame.stFrame.stVFrame.stVFrame;
        skelFrame.pUserData = (AX_VOID *)pPrivData;
        LOG_M_N(DETECTOR, "runskel vdGrp %d vdChn %d frame %lld pts %lld phy 0x%llx %dx%d stride %d blkId 0x%x", axFrame.nGrp, axFrame.nChn,
                axFrame.stFrame.stVFrame.stVFrame.u64SeqNum, axFrame.stFrame.stVFrame.stVFrame.u64PTS,
                axFrame.stFrame.stVFrame.stVFrame.u64PhyAddr[0], axFrame.stFrame.stVFrame.stVFrame.u32Width,
                axFrame.stFrame.stVFrame.stVFrame.u32Height, axFrame.stFrame.stVFrame.stVFrame.u32PicStride[0],
                axFrame.stFrame.stVFrame.stVFrame.u32BlkId[0]);

        auto &flag_vec = d_vec[nCurrGrp];
        //遍历三种算法是否使能
        for (int index = 0; index < ALGO_MAX_NUM; index++) {
            if (flag_vec[index]) {
                LOG_M_N(DETECTOR, "runskel SkelChn %d SkelType %d", pPrivData->nSkelChn, m_stAttr.tChnAttr[pPrivData->nSkelChn].nPPL[index]);
                pPrivData->nAlgoType = m_stAttr.tChnAttr[pPrivData->nSkelChn].nPPL[index];

                #if defined(__RECORD_VB_TIMESTAMP__)
                CTimestampHelper::RecordTimestamp(axFrame.stFrame.stVFrame.stVFrame, axFrame.nGrp, axFrame.nChn, TIMESTAMP_SKEL_PRE_SEND);
                #endif

                #ifdef __BOX_DEBUG__
                AX_S32 ret = 0;
                usleep(3000);
                m_skelData.giveback(pPrivData);
                #else
                AX_S32 ret = AX_SKEL_SendFrame(m_hSkel[pPrivData->nSkelChn][index], &skelFrame, -1);
                if (0 != ret) {
                    //送失败，把数据归还，退出当前通道送帧的循环。
                    LOG_M_E(DETECTOR, "%s: AX_SKEL_SendFrame(vdGrp %d, seq %lld, frame %lld) fail, ret = 0x%x", __func__, axFrame.nGrp,
                            axFrame.stFrame.stVFrame.stVFrame.u64SeqNum, skelFrame.nFrameId, ret);

                    m_skelData.giveback(pPrivData);
                    break;
                }
                #endif

                #if defined(__RECORD_VB_TIMESTAMP__)
                CTimestampHelper::RecordTimestamp(axFrame.stFrame.stVFrame.stVFrame, axFrame.nGrp, axFrame.nChn, TIMESTAMP_SKEL_POS_SEND);
                #endif
            }
        }
        /* release frame after done */
        axFrame.DecRef();
    }

    LOG_M_D(DETECTOR, "%s: ---", __func__);
}

AX_BOOL CDetector::Init(const DETECTOR_ATTR_T &stAttr, std::vector<MEDIA_INFO_T>& mediasMap) {
    LOG_M_D(DETECTOR, "%s: +++", __func__);

    if (0 == stAttr.nGrpCount) {
        LOG_M_E(DETECTOR, "%s: 0 grp", __func__);
        return AX_FALSE;
    }

    m_stAttr = stAttr;

    if (m_stAttr.nSkipRate <= 0) {
        m_stAttr.nSkipRate = 1;
    }

    /* define how many frames can skel to handle in parallel */
    //推理的帧数量是不变的。保存帧结果的数量也是可以不变的。
    constexpr AX_U32 PARALLEL_FRAME_COUNT = 2;
    m_skelData.reserve(stAttr.nGrpCount * PARALLEL_FRAME_COUNT);

    //都是预分配16个
    m_arrFrameQ = new (nothrow) CAXLockQ<CAXFrame>[stAttr.nGrpCount];
    if (!m_arrFrameQ) {
        LOG_M_E(DETECTOR, "%s: alloc queue fail", __func__);
        return AX_FALSE;
    } else {
        for (AX_U32 i = 0; i < stAttr.nGrpCount; ++i) {
            m_arrFrameQ[i].SetCapacity(-1);
        }
    }

    //1.先确定是否适配过
    AX_SKEL_INIT_PARAM_T stInit;
    memset(&stInit, 0, sizeof(stInit));
    stInit.pStrModelDeploymentPath = m_stAttr.strModelPath.c_str();
    //先确定路径是否ok，再建立map,绑定代码注册好的ppl
    AX_S32 ret = AX_SKEL_Init(&stInit);
    if (0 != ret) {
        LOG_M_E(DETECTOR, "%s: AX_SKEL_Init fail, ret = 0x%x", __func__, ret);

        delete[] m_arrFrameQ;
        m_arrFrameQ = nullptr;
        return AX_FALSE;
    }

    do {
        /* [3]: print SKEL version */
        const AX_SKEL_VERSION_INFO_T *pstVersion = NULL;
        ret = AX_SKEL_GetVersion(&pstVersion);
        if (0 != ret) {
            LOG_M_E(DETECTOR, "%s: AX_SKEL_GetVersion() fail, ret = 0x%x", __func__, ret);
        } else {
            if (pstVersion && pstVersion->pstrVersion) {
                LOG_M_I(DETECTOR, "SKEL version: %s", pstVersion->pstrVersion);
            }

            AX_SKEL_Release((AX_VOID *)pstVersion);
        }

        //制作一个标记位，表明现在当前路数的算法配置成功与否。
        d_vec.resize(stAttr.nGrpCount);
        int all_init_fail = false;
        //很重点这里，实际上nChannelNum才是真实的路数
        for (AX_U32 nChn = 0; nChn < m_stAttr.nChannelNum; ++nChn) {
            if (mediasMap[nChn].nMediaDelete == 1) continue;

            /* [5]: create SEKL handle */
            for (AX_U32 algo_id = 0; algo_id < ALGO_MAX_NUM; ++algo_id) {
                /* [4]: check whether has FHVP model or not */
                //没有找到相应的模型，就退出，这一路视频就可能缺少某个算法，或者没有算法、
                //这个应该要标记一下。
                const AX_SKEL_CAPABILITY_T *pstCapability = NULL;
                ret = AX_SKEL_GetCapability(&pstCapability);
                if (0 != ret) {
                    LOG_M_E(DETECTOR, "%s: AX_SKEL_GetCapability() fail, ret = 0x%x", __func__, ret);
                    break;
                } else {
                    AX_BOOL bFHVP{AX_FALSE};
                    if (pstCapability && 0 == pstCapability->nPPLConfigSize) {
                        LOG_M_E(DETECTOR, "%s: SKEL model has 0 PPL", __func__);
                    } else {
                        for (AX_U32 i = 0; i < pstCapability->nPPLConfigSize; ++i) {
                            if (AX_SKEL_PPL_HVCFP == pstCapability->pstPPLConfig[i].ePPL) {
                                bFHVP = AX_TRUE;
                                break;
                            }
                        }
                    }

                    AX_SKEL_Release((AX_VOID *)pstCapability);
                    if (!bFHVP) {
                        LOG_M_E(DETECTOR, "%s: SKEL not found skel model", __func__);
                        d_vec[nChn][algo_id] = false;
                        break;
                    }
                }

                //配置算法参数
                AX_SKEL_HANDLE_PARAM_T stHandleParam;
                memset(&stHandleParam, 0, sizeof(stHandleParam));
                stHandleParam.ePPL = (AX_SKEL_PPL_E)m_stAttr.tChnAttr[nChn].nPPL[algo_id];
                stHandleParam.nFrameDepth = m_stAttr.nDepth;
                stHandleParam.nFrameCacheDepth = 0;
                stHandleParam.nIoDepth = 0;
                stHandleParam.nWidth = m_stAttr.nW;
                stHandleParam.nHeight = m_stAttr.nH;
                if (m_stAttr.tChnAttr[nChn].nVNPU == AX_SKEL_NPU_DEFAULT) {
                    stHandleParam.nNpuType = AX_SKEL_NPU_DEFAULT;
                } else {
                    stHandleParam.nNpuType = (AX_U32)(1 << (m_stAttr.tChnAttr[nChn].nVNPU - 1));
                }

                AX_SKEL_CONFIG_T stConfig = {0};
                AX_SKEL_CONFIG_ITEM_T stItems[16] = {0};
                AX_U8 itemIndex = 0;
                stConfig.nSize = 0;
                stConfig.pstItems = &stItems[0];

                if (!m_stAttr.tChnAttr[nChn].bTrackEnable) {
                    // track_disable
                    stConfig.pstItems[itemIndex].pstrType = (AX_CHAR *)"track_disable";
                    AX_SKEL_COMMON_THRESHOLD_CONFIG_T stTrackDisableThreshold = {0};
                    stTrackDisableThreshold.fValue = 1;
                    stConfig.pstItems[itemIndex].pstrValue = (AX_VOID *)&stTrackDisableThreshold;
                    stConfig.pstItems[itemIndex].nValueSize = sizeof(AX_SKEL_COMMON_THRESHOLD_CONFIG_T);
                    itemIndex++;
                }

                // push_disable
                stConfig.pstItems[itemIndex].pstrType = (AX_CHAR *)"push_disable";
                AX_SKEL_COMMON_THRESHOLD_CONFIG_T stPushDisableThreshold = {0};
                stPushDisableThreshold.fValue = 1;
                stConfig.pstItems[itemIndex].pstrValue = (AX_VOID *)&stPushDisableThreshold;
                stConfig.pstItems[itemIndex].nValueSize = sizeof(AX_SKEL_COMMON_THRESHOLD_CONFIG_T);
                itemIndex++;

                stConfig.nSize = itemIndex;
                stHandleParam.stConfig = stConfig;

                //创建算法通道
                LOG_M_C(DETECTOR, "chn %d, ppl %d, depth %d, cache depth %d, %dx%d", nChn, stHandleParam.ePPL, stHandleParam.nFrameDepth,
                        stHandleParam.nFrameCacheDepth, stHandleParam.nWidth, stHandleParam.nHeight);
                ret = AX_SKEL_Create(&stHandleParam, &m_hSkel[nChn][algo_id]);

                if (0 != ret || NULL == m_hSkel[nChn]) {
                    LOG_M_E(DETECTOR, "%s: AX_SKEL_Create() fail, ret = 0x%x", __func__, ret);
                    d_vec[nChn][algo_id] = false;
                    break;
                }

                //注册回调
                /* [6]: register result callback */
                ret = AX_SKEL_RegisterResultCallback(m_hSkel[nChn][algo_id], SkelResultCallback, this);
                if (0 != ret) {
                    d_vec[nChn][algo_id] = false;
                    LOG_M_E(DETECTOR, "%s: AX_SKEL_RegisterResultCallback() fail, ret = 0x%x", __func__, ret);
                    break;
                }

                d_vec[nChn][algo_id] = true;
                //初始化成功过一次
                all_init_fail = false;
            }
        }

        //也就是所有通道都初始化失败
        if (all_init_fail == true && m_stAttr.nChannelNum != 0) {
            delete[] m_arrFrameQ;
            m_arrFrameQ = nullptr;
            return AX_FALSE;
        }

        LOG_M_D(DETECTOR, "%s: ---", __func__);
        return AX_TRUE;

    } while (0);

    DeInit();
    LOG_M_D(DETECTOR, "%s: ---", __func__);
    return AX_TRUE;
}

AX_BOOL CDetector::DeInit(AX_VOID) {
    LOG_M_D(DETECTOR, "%s: +++", __func__);

    AX_S32 ret;
    if (m_DetectThread.IsRunning()) {
        LOG_M_E(DETECTOR, "%s: detect thread is running", __func__);
        return AX_FALSE;
    }

    AX_U64 nBodyCnt = CDetectResult::GetInstance()->GetTotalCount(DETECT_TYPE_BODY);
    AX_U64 nVehiCnt = CDetectResult::GetInstance()->GetTotalCount(DETECT_TYPE_VEHICLE);
    AX_U64 nPlatCnt = CDetectResult::GetInstance()->GetTotalCount(DETECT_TYPE_PLATE);
    AX_U64 nCyclCnt = CDetectResult::GetInstance()->GetTotalCount(DETECT_TYPE_CYCLE);
    if (0 == nBodyCnt && 0 == nVehiCnt && 0 == nPlatCnt && 0 == nCyclCnt) {
        /* report "0 fhvp result" for UT case */
        LOG_M_C(DETECTOR, "0 fhvp result is detected");
    } else {
        LOG_M_C(DETECTOR, "total detect result => body = %lld, vehicle = %lld, plate = %lld, cycle = %d", nBodyCnt, nVehiCnt, nPlatCnt,
                nCyclCnt);
    }

    for (AX_U32 nChn = 0; nChn < m_stAttr.nChannelNum; ++nChn) {
        if (m_hSkel[nChn]) {
            for (AX_U32 algo_id = 0; algo_id < ALGO_MAX_NUM; ++algo_id) {
                if (d_vec[nChn][algo_id] == true) {
                    ret = AX_SKEL_Destroy(m_hSkel[nChn][algo_id]);
                    if (0 != ret) {
                        LOG_M_E(DETECTOR, "%s: AX_SKEL_Destroy() fail, ret = 0x%x", __func__, ret);
                        return AX_FALSE;
                    }
                }
            }
        }
    }

    ret = AX_SKEL_DeInit();
    if (0 != ret) {
        LOG_M_E(DETECTOR, "%s: AX_SKEL_DeInit() fail, ret = 0x%x", __func__, ret);
        return AX_FALSE;
    }

    if (m_arrFrameQ) {
        delete[] m_arrFrameQ;
        m_arrFrameQ = nullptr;
    }

    LOG_M_D(DETECTOR, "%s: ---", __func__);
    return AX_TRUE;
}

AX_BOOL CDetector::Start(AX_VOID) {
    LOG_M_D(DETECTOR, "%s: +++", __func__);

    if (!m_DetectThread.Start([this](AX_VOID *pArg) -> AX_VOID { RunDetect(pArg); }, this, "AppDetect", SCHED_FIFO, 99)) {
        LOG_M_E(DETECTOR, "%s: create detect thread fail", __func__);
        return AX_FALSE;
    }
    LOG_M_D(DETECTOR, "%s: ---", __func__);
    return AX_TRUE;
}

//指定id开始，实际上就是对m_arrFrameQ进行初始化和调用npu接口
AX_BOOL CDetector::StartId(int id, DETECTOR_CHN_ATTR_T det_attr) {
    LOG_M_W(DETECTOR, "%s: +++", __func__);
    do {
        auto &algo_type_arr = det_attr.nPPL;
        bool has_duplicate = false;
        std::unordered_set<int> seen;
        for (AX_U32 algo_id = 0; algo_id < ALGO_MAX_NUM; ++algo_id) {
            if (seen.find(algo_type_arr[algo_id]) != seen.end()) {
                has_duplicate = true;
                break;
            }
            seen.insert(algo_type_arr[algo_id]);

            if (has_duplicate) {
                continue;
            }

            const AX_SKEL_CAPABILITY_T *pstCapability = NULL;
            int ret = AX_SKEL_GetCapability(&pstCapability);
            auto new_ppl = (AX_SKEL_PPL_E)algo_type_arr[algo_id];
            if (0 != ret) {
                LOG_M_E(DETECTOR, "%s: AX_SKEL_GetCapability() fail, ret = 0x%x", __func__, ret);
                break;
            } else {
                AX_BOOL bPPL{AX_FALSE};
                if (pstCapability && 0 == pstCapability->nPPLConfigSize) {
                    LOG_M_E(DETECTOR, "%s: SKEL model has 0 PPL", __func__);
                } else {
                    for (AX_U32 i = 0; i < pstCapability->nPPLConfigSize; ++i) {
                        if (new_ppl == pstCapability->pstPPLConfig[i].ePPL) {
                            bPPL = AX_TRUE;
                            break;
                        }
                    }
                }
                AX_SKEL_Release((AX_VOID *)pstCapability);
                if (!bPPL) {
                    LOG_M_E(DETECTOR, "%s: SKEL not found skel model", __func__);
                    d_vec[id][algo_id] = false;
                    break;
                }
            }

            //配置算法参数
            AX_SKEL_HANDLE_PARAM_T stHandleParam;
            memset(&stHandleParam, 0, sizeof(stHandleParam));
            stHandleParam.ePPL = new_ppl;
            stHandleParam.nFrameDepth = m_stAttr.nDepth;
            stHandleParam.nFrameCacheDepth = 0;
            stHandleParam.nIoDepth = 0;
            stHandleParam.nWidth = m_stAttr.nW;
            stHandleParam.nHeight = m_stAttr.nH;
            if (det_attr.nVNPU == AX_SKEL_NPU_DEFAULT) {
                stHandleParam.nNpuType = AX_SKEL_NPU_DEFAULT;
            } else {
                stHandleParam.nNpuType = (AX_U32)(1 << (det_attr.nVNPU - 1));
            }
            AX_SKEL_CONFIG_T stConfig = {0};
            AX_SKEL_CONFIG_ITEM_T stItems[16] = {0};
            AX_U8 itemIndex = 0;
            stConfig.nSize = 0;
            stConfig.pstItems = &stItems[0];

            if (!det_attr.bTrackEnable) {
                // track_disable
                stConfig.pstItems[itemIndex].pstrType = (AX_CHAR *)"track_disable";
                AX_SKEL_COMMON_THRESHOLD_CONFIG_T stTrackDisableThreshold = {0};
                stTrackDisableThreshold.fValue = 1;
                stConfig.pstItems[itemIndex].pstrValue = (AX_VOID *)&stTrackDisableThreshold;
                stConfig.pstItems[itemIndex].nValueSize = sizeof(AX_SKEL_COMMON_THRESHOLD_CONFIG_T);
                itemIndex++;
            }
            // push_disable
            stConfig.pstItems[itemIndex].pstrType = (AX_CHAR *)"push_disable";
            AX_SKEL_COMMON_THRESHOLD_CONFIG_T stPushDisableThreshold = {0};
            stPushDisableThreshold.fValue = 0;
            stConfig.pstItems[itemIndex].pstrValue = (AX_VOID *)&stPushDisableThreshold;
            stConfig.pstItems[itemIndex].nValueSize = sizeof(AX_SKEL_COMMON_THRESHOLD_CONFIG_T);
            itemIndex++;

            stConfig.nSize = itemIndex;
            stHandleParam.stConfig = stConfig;

            //创建算法通道
            LOG_M_C(DETECTOR, "ppl %d, depth %d, cache depth %d, %dx%d", stHandleParam.ePPL, stHandleParam.nFrameDepth,
                    stHandleParam.nFrameCacheDepth, stHandleParam.nWidth, stHandleParam.nHeight);
            ret = AX_SKEL_Create(&stHandleParam, &m_hSkel[id][algo_id]);
            if (0 != ret || NULL == m_hSkel[id]) {
                LOG_M_E(DETECTOR, "%s: AX_SKEL_Create() fail, ret = 0x%x", __func__, ret);
                d_vec[id][algo_id] = false;
                break;
            }

            //注册回调
            /* [6]: register result callback */
            ret = AX_SKEL_RegisterResultCallback(m_hSkel[id][algo_id], SkelResultCallback, this);
            if (0 != ret) {
                d_vec[id][algo_id] = false;
                LOG_M_E(DETECTOR, "%s: AX_SKEL_RegisterResultCallback() fail, ret = 0x%x", __func__, ret);
                break;
            }
            d_vec[id][algo_id] = true;
        }
    } while (0);

    LOG_M_D(DETECTOR, "%s: ---", __func__);
    return AX_TRUE;
}

//指定id停止，但是不会停止detector主线程。
AX_BOOL CDetector::StopId(int id) {
    LOG_M_D(DETECTOR, "%s: +++", __func__);

    //先把消息队列清空，再删除skel
    if (m_arrFrameQ) {
        m_arrFrameQ[id].Wakeup();
        ClearQueue(id);
    }

    if (m_hSkel[id]) {
        for (AX_U32 algo_id = 0; algo_id < ALGO_MAX_NUM; ++algo_id) {
            if (d_vec[id][algo_id] == true) {
                int ret = AX_SKEL_Destroy(m_hSkel[id][algo_id]);
                if (0 != ret) {
                    LOG_M_E(DETECTOR, "%s: AX_SKEL_Destroy() fail, ret = 0x%x", __func__, ret);
                    return AX_FALSE;
                }
            }
        }
    }

    LOG_M_D(DETECTOR, "%s: ---", __func__);
    return AX_TRUE;
}

AX_BOOL CDetector::Clear(AX_VOID) {
    if (m_arrFrameQ) {
        for (AX_U32 i = 0; i < m_stAttr.nGrpCount; ++i) {
            ClearQueue(i);
        }
    }

    return AX_TRUE;
}

AX_BOOL CDetector::Stop(AX_VOID) {
    LOG_M_D(DETECTOR, "%s: +++", __func__);

    m_DetectThread.Stop();

    if (m_arrFrameQ) {
        for (AX_U32 i = 0; i < m_stAttr.nGrpCount; ++i) {
            m_arrFrameQ[i].Wakeup();
        }
    }

    m_DetectThread.Join();

    if (m_arrFrameQ) {
        for (AX_U32 i = 0; i < m_stAttr.nGrpCount; ++i) {
            ClearQueue(i);
        }
    }

    LOG_M_D(DETECTOR, "%s: ---", __func__);
    return AX_TRUE;
}

AX_BOOL CDetector::SendFrame(const CAXFrame &axFrame) {
    LOG_M_I(DETECTOR, "recvfrm vdGrp %d vdChn %d frame %lld pts %lld phy 0x%llx %dx%d stride %d blkId 0x%x", axFrame.nGrp, axFrame.nChn,
            axFrame.stFrame.stVFrame.stVFrame.u64SeqNum, axFrame.stFrame.stVFrame.stVFrame.u64PTS,
            axFrame.stFrame.stVFrame.stVFrame.u64PhyAddr[0], axFrame.stFrame.stVFrame.stVFrame.u32Width,
            axFrame.stFrame.stVFrame.stVFrame.u32Height, axFrame.stFrame.stVFrame.stVFrame.u32PicStride[0],
            axFrame.stFrame.stVFrame.stVFrame.u32BlkId[0]);

    if (SkipFrame(axFrame)) {
        LOG_M_I(DETECTOR, "dropfrm vdGrp %d vdChn %d frame %lld pts %lld phy 0x%llx %dx%d stride %d blkId 0x%x", axFrame.nGrp, axFrame.nChn,
                axFrame.stFrame.stVFrame.stVFrame.u64SeqNum, axFrame.stFrame.stVFrame.stVFrame.u64PTS,
                axFrame.stFrame.stVFrame.stVFrame.u64PhyAddr[0], axFrame.stFrame.stVFrame.stVFrame.u32Width,
                axFrame.stFrame.stVFrame.stVFrame.u32Height, axFrame.stFrame.stVFrame.stVFrame.u32PicStride[0],
                axFrame.stFrame.stVFrame.stVFrame.u32BlkId[0]);
        return AX_TRUE;
    }

    axFrame.IncRef();

#if defined(__RECORD_VB_TIMESTAMP__)
    CTimestampHelper::RecordTimestamp(axFrame.stFrame.stVFrame.stVFrame, axFrame.nGrp, axFrame.nChn, TIMESTAMP_SKEL_PUSH_FIFO);
#endif

    if (!m_arrFrameQ[axFrame.nGrp].Push(axFrame)) {
        LOG_M_E(DETECTOR, "%s: push frame %lld to q fail", __func__, axFrame.stFrame.stVFrame.stVFrame.u64SeqNum);
        axFrame.DecRef();
    }

    return AX_TRUE;
}

AX_BOOL CDetector::SkipFrame(const CAXFrame &axFrame) {
#ifdef __VDEC_PP_FRAME_CTRL__
    return AX_FALSE;
#else

    if (m_stAttr.nSkipRate <= 1) {
        return AX_FALSE;
    }

    return (1 == (axFrame.stFrame.stVFrame.stVFrame.u64SeqNum % m_stAttr.nSkipRate)) ? AX_FALSE : AX_TRUE;
#endif
}

AX_VOID CDetector::ClearQueue(AX_S32 nGrp) {
    AX_U32 nCount = m_arrFrameQ[nGrp].GetCount();
    if (nCount > 0) {
        CAXFrame axFrame;
        for (AX_U32 i = 0; i < nCount; ++i) {
            if (m_arrFrameQ[nGrp].Pop(axFrame, 0)) {
                axFrame.DecRef();
            }
        }
    }
}
#endif