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
#if defined(__RECORD_VB_TIMESTAMP__)
#include "TimestampHelper.hpp"
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
    //原始宽高，目标数量
    fhvp.nW = pstResult->nOriginalWidth;
    fhvp.nH = pstResult->nOriginalHeight;
    fhvp.nCount = pstResult->nObjectSize;
    //user data里面才是存了grp id相关，这样子才能解耦
    fhvp.nSeqNum = pPrivData->nSeqNum;
    fhvp.nGrpId = pPrivData->nGrpId;
    

    AX_U32 index = 0;
    //最大检测64个，遍历所有的数量
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

        //还有跟踪id
        fhvp.item[index].nTrackId = pstResult->pstObjectItems[i].nTrackId;
        //框
        fhvp.item[index].tBox = pstResult->pstObjectItems[i].stRect;
        index++;
    }

    fhvp.nCount = index;

    /* save fhvp result */
    //相当于保存为最终结构体，也就是设计的时候可以输出对应的结构体，然后中间可以多次转换。
    //之后再看一下哪里用了set之后的数据，应该是画框那里。所以多路传递就是根据nGrpId

    //首先这个是注册回调函数，调用CDetectResult的Set函数，将数据存起来。然后在其他地方使用get使用。
    //这里多线程肯定要加锁，里面的数据共享就是类的成员变量。
    //CDetectResult::GetInstance()实际上就是返回一个实例，也就是里面定义了一个static的类成员变量。
    //相当于检测类就单独实现，然后通过注册回调函数或者将结果放到一个共享数据类里面，然后加锁实现多线程。
    CDetectResult::GetInstance()->Set(pPrivData->nGrpId, fhvp);

    /* release fhvp result */
    //这个地方由用户释放pstResult
    AX_SKEL_Release((AX_VOID *)pstResult);

    /* giveback private data */
    //同样也释放pstResult的user_data
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
    //将线程封装了一层是很好的，可以控制优先级，线程名字和线程运行和停止之类的
    while (m_DetectThread.IsRunning()) {
        for (nCurrGrp = nNextGrp; nCurrGrp < TOTAL_GRP_COUNT; ++nCurrGrp) {
            //队列出队，里面进行加锁控制，队列里面的数据来源vdec
            if (m_arrFrameQ[nCurrGrp].Pop(axFrame, 0)) {
                //出去成功，skip设置为0
                nSkipCount = 0;
                break;
            }
            //如果没有frame出队，先休息一下
            if (++nSkipCount == TOTAL_GRP_COUNT) {
                this_thread::sleep_for(chrono::microseconds(1000));
                nSkipCount = 0;
            }
        }

        //相当于下次就跑到下一路去计算
        if (nCurrGrp == TOTAL_GRP_COUNT) {
            nNextGrp = 0;
            continue;
        } else {
            nNextGrp = nCurrGrp + 1;
            if (nNextGrp == TOTAL_GRP_COUNT) {
                nNextGrp = 0;
            }
        }

        //借了数据，为什么需要实现一个这样子的内存去保存
        SKEL_FRAME_PRIVATE_DATA_T *pPrivData = m_skelData.borrow();
        if (!pPrivData) {
            LOG_M_E(DETECTOR, "%s: borrow skel frame private data fail", __func__);
            axFrame.DecRef();
            continue;
        } else {
            //axFrame就是从每一路的队列里面取数据，序列号
            pPrivData->nSeqNum = axFrame.stFrame.stVFrame.stVFrame.u64SeqNum;
            //第几路
            pPrivData->nGrpId = axFrame.nGrp;
            //chanenl_num不一定等于grp_num，他进行取余，这个地方不理解，后面再看看
            pPrivData->nSkelChn = axFrame.nGrp % m_stAttr.nChannelNum;
        }

        AX_SKEL_FRAME_T skelFrame;
        skelFrame.nFrameId = ++nFrameId; /* skel recommends to unique frame id */
        skelFrame.nStreamId = axFrame.nGrp;
        //直接结构体赋值，肯定是不可以的，里面有数组
        skelFrame.stFrame = axFrame.stFrame.stVFrame.stVFrame;
        skelFrame.pUserData = (AX_VOID *)pPrivData;
        LOG_M_N(DETECTOR, "runskel vdGrp %d vdChn %d frame %lld pts %lld phy 0x%llx %dx%d stride %d blkId 0x%x", axFrame.nGrp, axFrame.nChn,
                axFrame.stFrame.stVFrame.stVFrame.u64SeqNum, axFrame.stFrame.stVFrame.stVFrame.u64PTS,
                axFrame.stFrame.stVFrame.stVFrame.u64PhyAddr[0], axFrame.stFrame.stVFrame.stVFrame.u32Width,
                axFrame.stFrame.stVFrame.stVFrame.u32Height, axFrame.stFrame.stVFrame.stVFrame.u32PicStride[0],
                axFrame.stFrame.stVFrame.stVFrame.u32BlkId[0]);

#if defined(__RECORD_VB_TIMESTAMP__)
        CTimestampHelper::RecordTimestamp(axFrame.stFrame.stVFrame.stVFrame, axFrame.nGrp, axFrame.nChn, TIMESTAMP_SKEL_PRE_SEND);
#endif

#ifdef __BOX_DEBUG__
        AX_S32 ret = 0;
        usleep(3000);
        m_skelData.giveback(pPrivData);
#else
        //发送frame很有意思，使用AX_SKEL_FRAME_T类型的结构体，m_hSkel就是对应handle
        //并且也是通过这个m_hSkel handle绑定了回调。
        AX_S32 ret = AX_SKEL_SendFrame(m_hSkel[pPrivData->nSkelChn], &skelFrame, -1);
#endif

#if defined(__RECORD_VB_TIMESTAMP__)
        CTimestampHelper::RecordTimestamp(axFrame.stFrame.stVFrame.stVFrame, axFrame.nGrp, axFrame.nChn, TIMESTAMP_SKEL_POS_SEND);
#endif

        /* release frame after done */
        //类似使用C++的智能指针，直接将frame进行--，其他地方进行释放。
        //这个底层实现肯定又比较麻烦。
        axFrame.DecRef();

        if (0 != ret) {
            LOG_M_E(DETECTOR, "%s: AX_SKEL_SendFrame(vdGrp %d, seq %lld, frame %lld) fail, ret = 0x%x", __func__, axFrame.nGrp,
                    axFrame.stFrame.stVFrame.stVFrame.u64SeqNum, skelFrame.nFrameId, ret);
            //这个地方估计会有bug，没有主动释放pPrivData，这个是错误的条件才进到这里面
            m_skelData.giveback(pPrivData);
        }
    }

    LOG_M_D(DETECTOR, "%s: ---", __func__);
}

//也就是上面一层是读取配置文件，转为结构体信息，然后底层再进行配置，一种解耦方式
AX_BOOL CDetector::Init(const DETECTOR_ATTR_T &stAttr) {
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
    constexpr AX_U32 PARALLEL_FRAME_COUNT = 2;
    //这个地方先保留并行的数据个数，与depth不相应符合
    m_skelData.reserve(stAttr.nGrpCount * PARALLEL_FRAME_COUNT);

    //根据grp num每一路创建一个队列，每一路是frame队列。先把业务逻辑考虑好，再考虑性能
    //就是CAXLockQ这个模板类，传递CAXFrame参数进行初始化，就是创建CAXFrame类型的队列
    m_arrFrameQ = new (nothrow) CAXLockQ<CAXFrame>[stAttr.nGrpCount];
    if (!m_arrFrameQ) {
        LOG_M_E(DETECTOR, "%s: alloc queue fail", __func__);
        return AX_FALSE;
    } else {
        for (AX_U32 i = 0; i < stAttr.nGrpCount; ++i) {
            m_arrFrameQ[i].SetCapacity(-1);
        }
    }

    /* [2]: SKEL init */
    //初始化runtime接口
    AX_SKEL_INIT_PARAM_T stInit;
    memset(&stInit, 0, sizeof(stInit));
    stInit.pStrModelDeploymentPath = m_stAttr.strModelPath.c_str();
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

        /* [4]: check whether has FHVP model or not */
        //检测是否有人车非模式
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

        //这个是遍历每一路
        for (AX_U32 nChn = 0; nChn < m_stAttr.nChannelNum; ++nChn) {
            /* [5]: create SEKL handle */
            //那么之前的m_stAttr还是作为中间变量，这个是作为程序设计的时候要注意的
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

            //相当于可能有多个itemIndex，一个是track_disable，另外一个是push disable
            stConfig.nSize = itemIndex;
            stHandleParam.stConfig = stConfig;

            //最终的目的是构造stHandleParam结构体。
            //相当于通过ini读取保存成一个结构体，然后中间被地方使用，再创建一个结构体，而不是直接包含旧的结构体。
            LOG_M_C(DETECTOR, "ppl %d, depth %d, cache depth %d, %dx%d", stHandleParam.ePPL, stHandleParam.nFrameDepth,
                    stHandleParam.nFrameCacheDepth, stHandleParam.nWidth, stHandleParam.nHeight);
            //创建m_hSkel handle，使用数组管理。
            ret = AX_SKEL_Create(&stHandleParam, &m_hSkel[nChn]);

            if (0 != ret || NULL == m_hSkel[nChn]) {
                LOG_M_E(DETECTOR, "%s: AX_SKEL_Create() fail, ret = 0x%x", __func__, ret);
                break;
            }

            /* [6]: register result callback */
            //注册回调函数，这个就类似于aisdk。其实push和track可以封装起来，每一路都注册回调
            ret = AX_SKEL_RegisterResultCallback(m_hSkel[nChn], SkelResultCallback, this);
            if (0 != ret) {
                LOG_M_E(DETECTOR, "%s: AX_SKEL_RegisterResultCallback() fail, ret = 0x%x", __func__, ret);
                break;
            }
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
    //线程停止
    m_DetectThread.Stop();

    //先把条件变量唤醒，不用再等待，不然一直在等待资源
    if (m_arrFrameQ) {
        for (AX_U32 i = 0; i < m_stAttr.nGrpCount; ++i) {
            m_arrFrameQ[i].Wakeup();
        }
    }
    //线程回收
    m_DetectThread.Join();

    //清空队列
    if (m_arrFrameQ) {
        for (AX_U32 i = 0; i < m_stAttr.nGrpCount; ++i) {
            ClearQueue(i);
        }
    }

    LOG_M_D(DETECTOR, "%s: ---", __func__);
    return AX_TRUE;
}

//这个地方会在vdec之后回调。
AX_BOOL CDetector::SendFrame(const CAXFrame &axFrame) {
    LOG_M_I(DETECTOR, "recvfrm vdGrp %d vdChn %d frame %lld pts %lld phy 0x%llx %dx%d stride %d blkId 0x%x", axFrame.nGrp, axFrame.nChn,
            axFrame.stFrame.stVFrame.stVFrame.u64SeqNum, axFrame.stFrame.stVFrame.stVFrame.u64PTS,
            axFrame.stFrame.stVFrame.stVFrame.u64PhyAddr[0], axFrame.stFrame.stVFrame.stVFrame.u32Width,
            axFrame.stFrame.stVFrame.stVFrame.u32Height, axFrame.stFrame.stVFrame.stVFrame.u32PicStride[0],
            axFrame.stFrame.stVFrame.stVFrame.u32BlkId[0]);

    //判断是否需要丢帧，如果丢帧，那么就直接退出。
    if (SkipFrame(axFrame)) {
        LOG_M_I(DETECTOR, "dropfrm vdGrp %d vdChn %d frame %lld pts %lld phy 0x%llx %dx%d stride %d blkId 0x%x", axFrame.nGrp, axFrame.nChn,
                axFrame.stFrame.stVFrame.stVFrame.u64SeqNum, axFrame.stFrame.stVFrame.stVFrame.u64PTS,
                axFrame.stFrame.stVFrame.stVFrame.u64PhyAddr[0], axFrame.stFrame.stVFrame.stVFrame.u32Width,
                axFrame.stFrame.stVFrame.stVFrame.u32Height, axFrame.stFrame.stVFrame.stVFrame.u32PicStride[0],
                axFrame.stFrame.stVFrame.stVFrame.u32BlkId[0]);
        return AX_TRUE;
    }

    //引用加一
    axFrame.IncRef();

#if defined(__RECORD_VB_TIMESTAMP__)
    CTimestampHelper::RecordTimestamp(axFrame.stFrame.stVFrame.stVFrame, axFrame.nGrp, axFrame.nChn, TIMESTAMP_SKEL_PUSH_FIFO);
#endif

    //这个地方就是把axFrame放在对应的队列里面，好像其他地方也有这个队列，不知道是不是同一个
    if (!m_arrFrameQ[axFrame.nGrp].Push(axFrame)) {
        LOG_M_E(DETECTOR, "%s: push frame %lld to q fail", __func__, axFrame.stFrame.stVFrame.stVFrame.u64SeqNum);
        //失败了再递减
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
