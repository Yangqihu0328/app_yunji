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
#include <exception>
#include "AXException.hpp"
#include "AppLogApi.h"
#include "DetectResult.hpp"
#include "ElapsedTimer.hpp"
#include "PcieAdapter.hpp"

#ifdef AX_SAMPLE
#include <fcntl.h>
#include <sys/mman.h>
#include <complex>
#include <math.h>
#include <algorithm>
#include <iostream>
#endif

using namespace std;
#define DETECTOR "SKEL"

static AX_VOID SkelResultCallback(AX_SKEL_HANDLE pHandle, AX_SKEL_RESULT_T *pstResult, AX_VOID *pUserData) {
    CDetector *pThis = (CDetector *)pUserData;
    if (!pThis) {
        THROW_AX_EXCEPTION("skel handle %p result callback user data is nil", pHandle);
    }

    SKEL_FRAME_PRIVATE_DATA_T *pPrivData = (SKEL_FRAME_PRIVATE_DATA_T *)(pstResult->pUserData);
    if (!pPrivData) {
        THROW_AX_EXCEPTION("skel handle %p frame private data is nil", pHandle);
    }

    DETECT_RESULT_T fhvp;
    // fhvp.nSeqNum = pstResult->nFrameId;
    fhvp.nW = pstResult->nOriginalWidth;
    fhvp.nH = pstResult->nOriginalHeight;
    fhvp.nSeqNum = pPrivData->nSeqNum;
    fhvp.nGrpId = pPrivData->nGrpId;
    fhvp.nCount = pstResult->nObjectSize;

    AX_U32 index = 0;
    for (AX_U32 i = 0; i < fhvp.nCount && index < MAX_DETECT_RESULT_COUNT; ++i) {
        if (pstResult->pstObjectItems[i].eTrackState != AX_SKEL_TRACK_STATUS_NEW &&
            pstResult->pstObjectItems[i].eTrackState != AX_SKEL_TRACK_STATUS_UPDATE) {
            continue;
        }

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
    CDetectResult::GetInstance()->Set(pPrivData->nGrpId, fhvp);

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

#ifdef AX_SAMPLE
        axdl_image_t tSrcFrame{0};
        tSrcFrame.nWidth = axFrame.stFrame.stVFrame.stVFrame.u32Width;
        tSrcFrame.nHeight = axFrame.stFrame.stVFrame.stVFrame.u32Height;
        tSrcFrame.pVir = axFrame.stFrame.stVFrame.stVFrame.u64VirAddr;
        tSrcFrame.pPhy = axFrame.stFrame.stVFrame.stVFrame.u64PhyAddr[0];
        // tSrcFrame.tStride_W = axFrame.stFrame.stVFrame.stVFrame.u32PicStride;
        tSrcFrame.nSize = axFrame.stFrame.stVFrame.stVFrame.u32FrameSize;
        inference(&tSrcFrame, nullptr);
        // 10. get result
        std::vector<detection> dets;
        dets = post_process(dets);
        fprintf(stdout, "--------------------------------------\n");

         DETECT_RESULT_T fhvp;
        // fhvp.nSeqNum = pstResult->nFrameId;
        fhvp.nW = 1920;
        fhvp.nH = 1080;
        fhvp.nSeqNum = axFrame.stFrame.stVFrame.stVFrame.u64SeqNum;
        fhvp.nGrpId = axFrame.nGrp;
        fhvp.nCount = dets.size();
        printf("dets size: %d\n", dets.size());
        
        AX_U32 index = 0;
        for (AX_U32 i = 0; i < fhvp.nCount && index < MAX_DETECT_RESULT_COUNT; ++i) {
            // if (pstResult->pstObjectItems[i].eTrackState != AX_SKEL_TRACK_STATUS_NEW &&
            //     pstResult->pstObjectItems[i].eTrackState != AX_SKEL_TRACK_STATUS_UPDATE) {
            //     continue;
            // }

            if (0 == strcmp(CLASS_NAMES[dets[i].cls], "person")) {
                fhvp.item[index].eType = DETECT_TYPE_BODY;
            } else if (0 == strcmp(CLASS_NAMES[dets[i].cls], "car")) {
                fhvp.item[index].eType = DETECT_TYPE_VEHICLE;
            } else if (0 == strcmp("pstResult->pstObjectItems[i].pstrObjectCategory", "cycle")) {
                fhvp.item[index].eType = DETECT_TYPE_CYCLE;
            } else if (0 == strcmp("pstResult->pstObjectItems[i].pstrObjectCategory", "face")) {
                fhvp.item[index].eType = DETECT_TYPE_FACE;
            } else if (0 == strcmp("pstResult->pstObjectItems[i].pstrObjectCategory", "plate")) {
                fhvp.item[index].eType = DETECT_TYPE_PLATE;
            } else {
                // LOG_M_W(DETECTOR, "unknown detect result %s of vdGrp %d frame %lld (skel %lld)",
                //         pstResult->pstObjectItems[i].pstrObjectCategory, fhvp.nGrpId, fhvp.nSeqNum, pstResult->nFrameId);
                fhvp.item[index].eType = DETECT_TYPE_UNKNOWN;
            }

            // fhvp.item[index].nTrackId = pstResult->pstObjectItems[i].nTrackId;
            
            
            fhvp.item[index].tBox = {dets[i].bbox.x, dets[i].bbox.y, dets[i].bbox.w, dets[i].bbox.h};
            // std::cout << "bbox x: " << dets[i].bbox.x << "\n"
            // << "bbox y: " << dets[i].bbox.y << "\n"
            // << "bbox w: " << dets[i].bbox.w << "\n"
            // << "bbox h: " << dets[i].bbox.h << std::endl;
            index++;
        }

        fhvp.nCount = index;
        printf("nCount: %d **********************\n", fhvp.nCount);
        /* save fhvp result */
        CDetectResult::GetInstance()->Set(fhvp.nGrpId, fhvp);
    
#else
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
                axFrame.stFrame.stVFrame.stVFrame.u64SeqNum, axFrame.stFrame.stVFrame.stVFrame.u64PTS, axFrame.stFrame.stVFrame.stVFrame.u64PhyAddr[0],
                axFrame.stFrame.stVFrame.stVFrame.u32Width, axFrame.stFrame.stVFrame.stVFrame.u32Height, axFrame.stFrame.stVFrame.stVFrame.u32PicStride[0],
                axFrame.stFrame.stVFrame.stVFrame.u32BlkId[0]);

        AX_S32 ret = AX_SKEL_SendFrame(m_hSkel[pPrivData->nSkelChn], &skelFrame, -1);

        /* release frame after done */
        axFrame.DecRef();

        if (0 != ret) {
            LOG_M_E(DETECTOR, "%s: AX_SKEL_SendFrame(vdGrp %d, seq %lld, frame %lld) fail, ret = 0x%x", __func__, axFrame.nGrp,
                    axFrame.stFrame.stVFrame.stVFrame.u64SeqNum, skelFrame.nFrameId, ret);

            m_skelData.giveback(pPrivData);
        }
#endif
    }

    LOG_M_D(DETECTOR, "%s: ---", __func__);

}

AX_BOOL CDetector::Init(const DETECTOR_ATTR_T &stAttr, AX_BOOL bSimulateDetRets /*= AX_FALSE*/) {
    LOG_MM_C(DETECTOR, "+++");

    if (0 == stAttr.nGrpCount) {
        LOG_M_E(DETECTOR, "%s: 0 grp", __func__);
        return AX_FALSE;
    }

    m_stAttr = stAttr;
    m_bSimulateDetRets = bSimulateDetRets;

    if (m_stAttr.nSkipRate <= 0) {
        m_stAttr.nSkipRate = 1;
    }

    m_arrFrameQ = new (nothrow) CAXLockQ<CAXFrame>[stAttr.nGrpCount];
    if (!m_arrFrameQ) {
        LOG_M_E(DETECTOR, "%s: alloc queue fail", __func__);
        return AX_FALSE;
    } else {
        AX_S32 nBufQDepth = m_stAttr.nDepth / stAttr.nGrpCount;
        for (AX_U32 i = 0; i < stAttr.nGrpCount; ++i) {
            m_arrFrameQ[i].SetCapacity(nBufQDepth);
        }
    }

    /* define how many frames can skel to handle in parallel */
    constexpr AX_U32 PARALLEL_FRAME_COUNT = 2;
    m_skelData.reserve(stAttr.nGrpCount * PARALLEL_FRAME_COUNT);

    LOG_MM_C(DETECTOR, "---");

    return AX_TRUE;
}

AX_BOOL CDetector::DeInit(AX_VOID) {
    LOG_MM_C(DETECTOR, "+++");

    if (m_arrFrameQ) {
        delete[] m_arrFrameQ;
        m_arrFrameQ = nullptr;
    }

    m_skelData.destory();

    LOG_MM_C(DETECTOR, "---");
    return AX_TRUE;
}

AX_BOOL CDetector::InitSkel() {
    LOG_MM_C(DETECTOR, "+++");

    /* [1]: SKEL init */
    AX_SKEL_INIT_PARAM_T stInit;
    memset(&stInit, 0, sizeof(stInit));
    stInit.pStrModelDeploymentPath = m_stAttr.szModelPath;
    AX_S32 ret = AX_SKEL_Init(&stInit);
    if (0 != ret) {
        LOG_M_E(DETECTOR, "%s: AX_SKEL_Init fail, ret = 0x%x", __func__, ret);
        return AX_FALSE;
    }

    do {
        /* [2]: print SKEL version */
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

        /* [3]: check whether has FHVP model or not */
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
                LOG_M_E(DETECTOR, "%s: SKEL not found FHVP model", __func__);
                break;
            }
        }

        for (AX_U32 nChn = 0; nChn < m_stAttr.nChannelNum; ++nChn) {
            /* [4]: create SEKL handle */
            AX_SKEL_HANDLE_PARAM_T stHandleParam;
            memset(&stHandleParam, 0, sizeof(stHandleParam));
            stHandleParam.ePPL = (AX_SKEL_PPL_E)m_stAttr.tChnAttr[nChn].nPPL;
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

            LOG_M_C(DETECTOR, "ppl %d, depth %d, cache depth %d, %dx%d", stHandleParam.ePPL, stHandleParam.nFrameDepth,
                    stHandleParam.nFrameCacheDepth, stHandleParam.nWidth, stHandleParam.nHeight);
            ret = AX_SKEL_Create(&stHandleParam, &m_hSkel[nChn]);

            if (0 != ret || NULL == m_hSkel[nChn]) {
                LOG_M_E(DETECTOR, "%s: AX_SKEL_Create() fail, ret = 0x%x", __func__, ret);
                break;
            }

            /* [5]: register result callback */
            ret = AX_SKEL_RegisterResultCallback(m_hSkel[nChn], SkelResultCallback, this);
            if (0 != ret) {
                LOG_M_E(DETECTOR, "%s: AX_SKEL_RegisterResultCallback() fail, ret = 0x%x", __func__, ret);
                break;
            }
        }

        LOG_MM_C(DETECTOR, "---");
        return AX_TRUE;

    } while (0);

    DeInitSkel();

    LOG_MM_C(DETECTOR, "---");

    return AX_FALSE;
}

AX_BOOL CDetector::DeInitSkel(AX_VOID) {
    LOG_MM_C(DETECTOR, "+++");

    AX_S32 ret;
    if (m_DetectThread.IsRunning()) {
        LOG_M_E(DETECTOR, "%s: detect thread is running", __func__);
        return AX_FALSE;
    }

    LOG_M_C(DETECTOR, "total detect result => body = %lld, vehicle = %lld, plate = %lld, cycle = %d",
            CDetectResult::GetInstance()->GetTotalCount(DETECT_TYPE_BODY), CDetectResult::GetInstance()->GetTotalCount(DETECT_TYPE_VEHICLE),
            CDetectResult::GetInstance()->GetTotalCount(DETECT_TYPE_PLATE), CDetectResult::GetInstance()->GetTotalCount(DETECT_TYPE_CYCLE));

    for (AX_U32 nChn = 0; nChn < m_stAttr.nChannelNum; ++nChn) {
        if (m_hSkel[nChn]) {
            ret = AX_SKEL_Destroy(m_hSkel[nChn]);
            if (0 != ret) {
                LOG_M_E(DETECTOR, "%s: AX_SKEL_Destroy() fail, ret = 0x%x", __func__, ret);
                return AX_FALSE;
            }
        }
    }

    ret = AX_SKEL_DeInit();
    if (0 != ret) {
        LOG_M_E(DETECTOR, "%s: AX_SKEL_DeInit() fail, ret = 0x%x", __func__, ret);
        return AX_FALSE;
    }

    LOG_MM_C(DETECTOR, "---");

    return AX_TRUE;
}

#ifdef AX_SAMPLE
#define AX_CMM_ALIGN_SIZE 128

const char *AX_CMM_SESSION_NAME = "ax-pipeline-npu";

typedef enum
{
    AX_ENGINE_ABST_DEFAULT = 0,
    AX_ENGINE_ABST_CACHED = 1,
} AX_ENGINE_ALLOC_BUFFER_STRATEGY_T;

typedef std::pair<AX_ENGINE_ALLOC_BUFFER_STRATEGY_T, AX_ENGINE_ALLOC_BUFFER_STRATEGY_T> INPUT_OUTPUT_ALLOC_STRATEGY;

void free_io_index(AX_ENGINE_IO_BUFFER_T *io_buf, int index)
{
    for (int i = 0; i < index; ++i)
    {
        AX_ENGINE_IO_BUFFER_T *pBuf = io_buf + i;
        AX_SYS_MemFree(pBuf->phyAddr, pBuf->pVirAddr);
    }
}

void free_io(AX_ENGINE_IO_T *io)
{
    for (size_t j = 0; j < io->nInputSize; ++j)
    {
        AX_ENGINE_IO_BUFFER_T *pBuf = io->pInputs + j;
        AX_SYS_MemFree(pBuf->phyAddr, pBuf->pVirAddr);
    }
    for (size_t j = 0; j < io->nOutputSize; ++j)
    {
        AX_ENGINE_IO_BUFFER_T *pBuf = io->pOutputs + j;
        AX_SYS_MemFree(pBuf->phyAddr, pBuf->pVirAddr);
    }
    delete[] io->pInputs;
    delete[] io->pOutputs;
}

static inline int prepare_io(AX_ENGINE_IO_INFO_T *info, AX_ENGINE_IO_T *io_data, INPUT_OUTPUT_ALLOC_STRATEGY strategy)
{
    memset(io_data, 0, sizeof(*io_data));
    io_data->pInputs = new AX_ENGINE_IO_BUFFER_T[info->nInputSize];
    memset(io_data->pInputs, 0, sizeof(AX_ENGINE_IO_BUFFER_T) * info->nInputSize);
    io_data->nInputSize = info->nInputSize;

    auto ret = 0;
    for (int i = 0; i < (int)info->nInputSize; ++i)
    {
        auto meta = info->pInputs[i];
        auto buffer = &io_data->pInputs[i];
        if (strategy.first == AX_ENGINE_ABST_CACHED)
        {
            ret = AX_SYS_MemAllocCached((AX_U64*)(&buffer->phyAddr), &buffer->pVirAddr, meta.nSize, AX_CMM_ALIGN_SIZE, (const AX_S8*)(AX_CMM_SESSION_NAME));
        }
        else
        {
            ret = AX_SYS_MemAlloc((AX_U64*)(&buffer->phyAddr), &buffer->pVirAddr, meta.nSize, AX_CMM_ALIGN_SIZE, (const AX_S8*)(AX_CMM_SESSION_NAME));
        }

        if (ret != 0)
        {
            free_io_index(io_data->pInputs, i);
            fprintf(stderr, "Allocate input{%d} { phy: %p, vir: %p, size: %lu Bytes }. fail \n", i, (void*)buffer->phyAddr, buffer->pVirAddr, (long)meta.nSize);
            return ret;
        }
    }

    io_data->pOutputs = new AX_ENGINE_IO_BUFFER_T[info->nOutputSize];
    memset(io_data->pOutputs, 0, sizeof(AX_ENGINE_IO_BUFFER_T) * info->nOutputSize);
    io_data->nOutputSize = info->nOutputSize;
    for (int i = 0; i < (int)info->nOutputSize; ++i)
    {
        auto meta = info->pOutputs[i];
        auto buffer = &io_data->pOutputs[i];
        buffer->nSize = meta.nSize;
        if (strategy.second == AX_ENGINE_ABST_CACHED)
        {
            ret = AX_SYS_MemAllocCached((AX_U64*)(&buffer->phyAddr), &buffer->pVirAddr, meta.nSize, AX_CMM_ALIGN_SIZE, (const AX_S8*)(AX_CMM_SESSION_NAME));
        }
        else
        {
            ret = AX_SYS_MemAlloc((AX_U64*)(&buffer->phyAddr), &buffer->pVirAddr, meta.nSize, AX_CMM_ALIGN_SIZE, (const AX_S8*)(AX_CMM_SESSION_NAME));
        }
        if (ret != 0)
        {
            fprintf(stderr, "Allocate output{%d} { phy: %p, vir: %p, size: %lu Bytes }. fail \n", i, (void*)buffer->phyAddr, buffer->pVirAddr, (long)meta.nSize);
            free_io_index(io_data->pInputs, io_data->nInputSize);
            free_io_index(io_data->pOutputs, i);
            return ret;
        }
    }

    return 0;
}

AX_BOOL CDetector::InitEngineHandle() {
    if (m_handle)
    {
        return AX_FALSE;
    }
    m_handle = new ax_joint_runner_ax650_handle_t;

    // 1. init engine
    int ret = 0;

    // 2. load model
    auto *file_fp = fopen(m_stAttr.szModelPath, "r");
    if (!file_fp)
    {
        printf("Read model(%s) file failed.\n", m_stAttr.szModelPath);
        return AX_FALSE;
    }
    fseek(file_fp, 0, SEEK_END);
    int model_size = ftell(file_fp);
    fclose(file_fp);
    int fd = open(m_stAttr.szModelPath, O_RDWR, 0644);
    void *mmap_add = mmap(NULL, model_size, PROT_WRITE, MAP_SHARED, fd, 0);

    // 3. create handle
    printf("MODEL PATH: %s\n", m_stAttr.szModelPath);
    
    ret = AX_ENGINE_CreateHandle(&m_handle->handle, mmap_add, model_size);
    if (0 != ret)
    {
        printf("AX_ENGINE_CreateHandle 0x%x", ret);
        return AX_FALSE;
    }
    printf("Engine creating handle is done.\n");
    munmap(mmap_add, model_size);

    // 4. create context
    ret = AX_ENGINE_CreateContext(m_handle->handle);
    if (0 != ret)
    {
        printf("AX_ENGINE_CreateContext 0x%x", ret);
        return AX_FALSE;
    }
    printf("Engine creating context is done.\n");

    // 5. set io

    ret = AX_ENGINE_GetIOInfo(m_handle->handle, &m_handle->io_info);
    if (0 != ret)
    {
        return AX_FALSE;
    }
    printf("Engine get io info is done. \n");

    // 6. alloc io
    ret = prepare_io(m_handle->io_info, &m_handle->io_data, std::make_pair(AX_ENGINE_ABST_DEFAULT, AX_ENGINE_ABST_DEFAULT));
    if (0 != ret)
    {
        return AX_FALSE;
    }
    printf("Engine alloc io is done. \n");

    for (size_t i = 0; i < m_handle->io_info->nOutputSize; i++)
    {  
        ax_runner_tensor_t tensor;
        tensor.nIdx = i;
        tensor.sName = std::string(m_handle->io_info->pOutputs[i].pName);
        tensor.nSize = m_handle->io_info->pOutputs[i].nSize;
        
        for (size_t j = 0; j < m_handle->io_info->pOutputs[i].nShapeSize; j++)
        {
            tensor.vShape.push_back(m_handle->io_info->pOutputs[i].pShape[j]);
        }
        tensor.phyAddr = m_handle->io_data.pOutputs[i].phyAddr;
        tensor.pVirAddr = m_handle->io_data.pOutputs[i].pVirAddr;
        mtensors.push_back(tensor);
    }

    for (size_t i = 0; i < m_handle->io_info->nInputSize; i++)
    {
        ax_runner_tensor_t tensor;
        tensor.nIdx = i;
        tensor.sName = std::string(m_handle->io_info->pInputs[i].pName);
        tensor.nSize = m_handle->io_info->pInputs[i].nSize;
        for (size_t j = 0; j < m_handle->io_info->pInputs[i].nShapeSize; j++)
        {
            tensor.vShape.push_back(m_handle->io_info->pInputs[i].pShape[j]);
        }
        tensor.phyAddr = m_handle->io_data.pInputs[i].phyAddr;
        tensor.pVirAddr = m_handle->io_data.pInputs[i].pVirAddr;
        minput_tensors.push_back(tensor);
    }
    return AX_TRUE;
}

AX_BOOL CDetector::DeInitEngineHandle() {
    if (m_handle && m_handle->handle)
    {
        free_io(&m_handle->io_data);
        AX_ENGINE_DestroyHandle(m_handle->handle);
    }
    delete m_handle;
    m_handle = nullptr;
    AX_ENGINE_Deinit();
}

int CDetector::inference(axdl_image_t *pstFrame, const axdl_bbox_t *crop_resize_box)
{   
    memcpy(minput_tensors[0].pVirAddr, pstFrame->pVir, minput_tensors[0].nSize);
    return AX_ENGINE_RunSync(m_handle->handle, &m_handle->io_data);
}

typedef struct {
  ax_runner_tensor_t *output;
  int num_anchors, h, w, bbox_len, batch = 1, layer_idx;
} detectLayer;

static const char *coco_names[] = {
    "person",        "bicycle",       "car",           "motorbike",
    "aeroplane",     "bus",           "train",         "truck",
    "boat",          "traffic light", "fire hydrant",  "stop sign",
    "parking meter", "bench",         "bird",          "cat",
    "dog",           "horse",         "sheep",         "cow",
    "elephant",      "bear",          "zebra",         "giraffe",
    "backpack",      "umbrella",      "handbag",       "tie",
    "suitcase",      "frisbee",       "skis",          "snowboard",
    "sports ball",   "kite",          "baseball bat",  "baseball glove",
    "skateboard",    "surfboard",     "tennis racket", "bottle",
    "wine glass",    "cup",           "fork",          "knife",
    "spoon",         "bowl",          "banana",        "apple",
    "sandwich",      "orange",        "broccoli",      "carrot",
    "hot dog",       "pizza",         "donut",         "cake",
    "chair",         "sofa",          "pottedplant",   "bed",
    "diningtable",   "toilet",        "tvmonitor",     "laptop",
    "mouse",         "remote",        "keyboard",      "cell phone",
    "microwave",     "oven",          "toaster",       "sink",
    "refrigerator",  "book",          "clock",         "vase",
    "scissors",      "teddy bear",    "hair drier",    "toothbrush"};

static float anchors_[3][3][2] = {{{10, 13}, {16, 30}, {33, 23}},
                                  {{30, 61}, {62, 45}, {59, 119}},
                                  {{116, 90}, {156, 198}, {373, 326}}};

template <typename T> int argmax(const T *data, size_t len, size_t stride = 1) {
  int maxIndex = 0;
  for (size_t i = 1; i < len; i++) {
    int idx = i * stride;
    if (data[maxIndex * stride] < data[idx]) {
      maxIndex = i;
    }
  }
  return maxIndex;
}

static float sigmoid(float x) { return 1.0 / (1 + expf(-x)); }

float calIou(box a, box b) {
    float area1 = a.w * a.h;
    float area2 = b.w * b.h;
    float wi = std::min((a.x + a.w), (b.x + b.w)) -
                    std::max((a.x), (b.x));
    float hi = std::min((a.y + a.h), (b.y + b.h)) -
                    std::max((a.y), (b.y));
    float area_i = std::max(wi, 0.0f) * std::max(hi, 0.0f);
    return area_i / (area1 + area2 - area_i);
}

void correctYoloBoxes(detection *dets, int det_num, int image_h, int image_w,
                      int input_height, int input_width) {
  int restored_w;
  int restored_h;
  float resize_ratio;
  if (((float)input_width / image_w) < ((float)input_height / image_h)) {
    restored_w = input_width;
    restored_h = (image_h * input_width) / image_w;
  } else {
    restored_h = input_height;
    restored_w = (image_w * input_height) / image_h;
  }
  resize_ratio = ((float)image_w / restored_w);

  for (int i = 0; i < det_num; ++i) {
    box bbox = dets[i].bbox;
    int b = dets[i].batch_idx;
    bbox.x = (bbox.x - (input_width - restored_w) / 2.) * resize_ratio;
    bbox.y = (bbox.y - (input_height - restored_h) / 2.) * resize_ratio;
    bbox.w *= resize_ratio;
    bbox.h *= resize_ratio;
    dets[i].bbox = bbox;
  }
}

void NMS(detection *dets, int *total, float thresh) {
  if (*total) {
    std::sort(dets, dets + *total - 1,
              [](detection &a, detection &b) { return b.score < a.score; });
    int new_count = *total;
    for (int i = 0; i < *total; ++i) {
      detection &a = dets[i];
      if (a.score == 0)
        continue;
      for (int j = i + 1; j < *total; ++j) {
        detection &b = dets[j];
        if (dets[i].batch_idx == dets[j].batch_idx && b.score != 0 &&
            dets[i].cls == dets[j].cls && calIou(a.bbox, b.bbox) > thresh) {
          b.score = 0;
          new_count--;
        }
      }
    }
    for (int i = 0, j = 0; i < *total && j < new_count; ++i) {
      detection &a = dets[i];
      if (a.score == 0)
        continue;
      dets[j] = dets[i];
      ++j;
    }
    *total = new_count;
  }
}

void generate_proposals_yolov5(int stride, const float* feat, float prob_threshold, std::vector<detection>& objects,
                                          int letterbox_cols, int letterbox_rows, const float* anchors, float prob_threshold_unsigmoid, int cls_num = 80)
{
    int anchor_num = 3;
    int feat_w = letterbox_cols / stride;
    int feat_h = letterbox_rows / stride;
    int anchor_group;
    if (stride == 8)
        anchor_group = 1;
    if (stride == 16)
        anchor_group = 2;
    if (stride == 32)
        anchor_group = 3;

    auto feature_ptr = feat;

    for (int h = 0; h <= feat_h - 1; h++)
    {
        for (int w = 0; w <= feat_w - 1; w++)
        {
            for (int a = 0; a <= anchor_num - 1; a++)
            {
                if (feature_ptr[4] < prob_threshold_unsigmoid)
                {
                    feature_ptr += (cls_num + 5);
                    continue;
                }

                //process cls score
                int class_index = 0;
                float class_score = 0;
                for (int s = 0; s <= cls_num - 1; s++)
                {
                    float score = feature_ptr[s + 5];
                    if (score > class_score)
                    {
                        class_index = s;
                        class_score = score;
                    }
                }
                //process box score
                float box_score = feature_ptr[4];
                float final_score = sigmoid(box_score) * sigmoid(class_score);

                if (final_score >= prob_threshold)
                {
                    float dx = sigmoid(feature_ptr[0]);
                    float dy = sigmoid(feature_ptr[1]);
                    float dw = sigmoid(feature_ptr[2]);
                    float dh = sigmoid(feature_ptr[3]);
                    float pred_cx = (dx * 2.0f - 0.5f + w) * stride;
                    float pred_cy = (dy * 2.0f - 0.5f + h) * stride;
                    float anchor_w = anchors[(anchor_group - 1) * 6 + a * 2 + 0];
                    float anchor_h = anchors[(anchor_group - 1) * 6 + a * 2 + 1];
                    float pred_w = dw * dw * 4.0f * anchor_w;
                    float pred_h = dh * dh * 4.0f * anchor_h;
                    float x0 = pred_cx - pred_w * 0.5f;
                    float y0 = pred_cy - pred_h * 0.5f;
                    float x1 = pred_cx + pred_w * 0.5f;
                    float y1 = pred_cy + pred_h * 0.5f;
                    
                    
                    detection obj;
                    obj.bbox.x = x0;
                    obj.bbox.y = y0;
                    obj.bbox.w = x1 - x0;
                    obj.bbox.h = y1 - y0;
                    obj.cls = class_index;
                    obj.score = final_score;
                    objects.push_back(obj);
                }

                feature_ptr += (cls_num + 5);
            }
        }
    }
}

std::vector<detection> CDetector::post_process(std::vector<detection> dets)
{
    const float ANCHORS[18] = {10, 13, 16, 30, 33, 23, 30, 61, 62, 45, 59, 119, 116, 90, 156, 198, 373, 326};
    AX_ENGINE_IO_INFO_T* io_info = m_handle->io_info;
    AX_ENGINE_IO_T *io_data = &m_handle->io_data;
    float prob_threshold_u_sigmoid = -1.0f * (float)std::log((1.0f / PROB_THRESHOLD) - 1.0f);
    for (uint32_t i = 0; i < io_info->nOutputSize; ++i)
    {
        auto& output = io_data->pOutputs[i];
        auto& info = io_info->pOutputs[i];
        auto ptr = (float*)output.pVirAddr;

        int32_t stride = (1 << i) * 8;
        generate_proposals_yolov5(stride, ptr, PROB_THRESHOLD, dets, 640, 640, ANCHORS, prob_threshold_u_sigmoid);
    }
    std::cout << dets.size() << std::endl;
    int count = dets.size();

    NMS(dets.data(), &count, 0.5);
    std::vector<detection> res;
    for (int i = 0; i < count; i++) {
        res.push_back(dets[i]);
    }
    return res;
}
#endif

AX_BOOL CDetector::Start(AX_VOID) {
    LOG_MM_C(DETECTOR, "+++");
    
    #ifdef AX_SAMPLE
    InitEngineHandle();
    #else
    if (!InitSkel()) {
        return AX_FALSE;
    }
    #endif

    if (!m_DetectThread.Start([this](AX_VOID *pArg) -> AX_VOID { RunDetect(pArg); }, this, "AppDetect", SCHED_FIFO, 99)) {
        LOG_M_E(DETECTOR, "%s: create detect thread fail", __func__);
        return AX_FALSE;
    }

    LOG_MM_C(DETECTOR, "---");
    return AX_TRUE;
}

AX_BOOL CDetector::Stop(AX_VOID) {
    LOG_MM_C(DETECTOR, "+++");

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
    
    #ifdef AX_SAMPLE
    DeInitEngineHandle();
    #else
    if (!DeInitSkel()) {
        return AX_FALSE;
    }
    #endif

    LOG_MM_C(DETECTOR, "---");
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

AX_BOOL CDetector::ThreadSwitchAttr(AI_CARD_AI_SWITCH_ATTR_T &tNewAttr) {
    m_stSwitchingAttr = m_stAttr;
    strcpy(m_stSwitchingAttr.szModelPath, tNewAttr.szModelPath);
    m_stSwitchingAttr.nChannelNum = tNewAttr.nChannelNum;
    for (AX_U8 i = 0; i < m_stSwitchingAttr.nChannelNum; i++) {
        m_stSwitchingAttr.tChnAttr[i].nPPL = tNewAttr.arrChnParam[i].nPPL;
        m_stSwitchingAttr.tChnAttr[i].bTrackEnable = tNewAttr.arrChnParam[i].bTrackEnable;
        m_stSwitchingAttr.tChnAttr[i].nVNPU = tNewAttr.arrChnParam[i].nVNPU;
    }

    if (!m_bAiSwitching) {
        thread t([](CDetector *p) { p->SwitchAiAttr(); }, this);
        t.detach();
    } else {
        LOG_M_W(DETECTOR, "Last switching is in progress, ignore this time.");
    }

    return AX_TRUE;
}

AX_BOOL CDetector::SwitchAiAttr() {
    LOG_MM_C(DETECTOR, "+++");

    LOG_MM_C(DETECTOR, "============== AI Switch Started ==============");
    m_bAiSwitching = AX_TRUE;

    if (!Stop()) {
        CPcieAdapter::GetInstance()->SendCtrlResult(0);
        LOG_MM_E(DETECTOR, "Stop detector failed when switching alg.");
        return AX_FALSE;
    }

    if (!Start()) {
        CPcieAdapter::GetInstance()->SendCtrlResult(0);
        LOG_MM_E(DETECTOR, "Start detector failed when switching alg.");
        return AX_FALSE;
    }

    CPcieAdapter::GetInstance()->SendCtrlResult(1);

    m_bAiSwitching = AX_FALSE;
    LOG_MM_W(DETECTOR, "============== AI Switch Finished ==============");

    LOG_MM_C(DETECTOR, "---");
    return AX_TRUE;
}

AX_BOOL CDetector::SendFrame(const CAXFrame &axFrame) {
    if (m_bSimulateDetRets) {
        return AX_TRUE;
    }

    if (m_bAiSwitching) {
        return AX_TRUE;
    }

    if (!m_DetectThread.IsRunning()) {
        return AX_TRUE;
    }

    LOG_M_I(DETECTOR, "recvfrm vdGrp %d vdChn %d frame %lld pts %lld phy 0x%llx %dx%d stride %d blkId 0x%x", axFrame.nGrp, axFrame.nChn,
            axFrame.stFrame.stVFrame.stVFrame.u64SeqNum, axFrame.stFrame.stVFrame.stVFrame.u64PTS, axFrame.stFrame.stVFrame.stVFrame.u64PhyAddr[0],
            axFrame.stFrame.stVFrame.stVFrame.u32Width, axFrame.stFrame.stVFrame.stVFrame.u32Height, axFrame.stFrame.stVFrame.stVFrame.u32PicStride[0],
            axFrame.stFrame.stVFrame.stVFrame.u32BlkId[0]);

    if (SkipFrame(axFrame)) {
        LOG_M_I(DETECTOR, "dropfrm vdGrp %d vdChn %d frame %lld pts %lld phy 0x%llx %dx%d stride %d blkId 0x%x", axFrame.nGrp, axFrame.nChn,
                axFrame.stFrame.stVFrame.stVFrame.u64SeqNum, axFrame.stFrame.stVFrame.stVFrame.u64PTS, axFrame.stFrame.stVFrame.stVFrame.u64PhyAddr[0],
                axFrame.stFrame.stVFrame.stVFrame.u32Width, axFrame.stFrame.stVFrame.stVFrame.u32Height, axFrame.stFrame.stVFrame.stVFrame.u32PicStride[0],
                axFrame.stFrame.stVFrame.stVFrame.u32BlkId[0]);
        return AX_TRUE;
    }

    axFrame.IncRef();

    if (!m_arrFrameQ[axFrame.nGrp].Push(axFrame)) {
        LOG_M_W(DETECTOR, "%s: push frame %lld to q full", __func__, axFrame.stFrame.stVFrame.stVFrame.u64SeqNum);
        axFrame.DecRef();
    }

    return AX_TRUE;
}

AX_BOOL CDetector::SkipFrame(const CAXFrame &axFrame) {
    return (1 == (axFrame.stFrame.stVFrame.stVFrame.u64SeqNum % m_stAttr.nSkipRate)) ? AX_FALSE : AX_TRUE;
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
