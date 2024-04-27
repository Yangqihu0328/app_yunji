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
#include <vector>
#include "AXFrame.hpp"
#include "AXLockQ.hpp"
#include "AXResource.hpp"
#include "AXThread.hpp"
#include "PcieAdapter.hpp"
#include "ax_skel_api.h"

#define AX_SAMPLE 1

#ifdef AX_SAMPLE
#include "ax_engine_api.h"
#endif

#define DETECTOR_MAX_CHN_NUM 3

typedef struct DETECTOR_CHN_ATTR_S {
    AX_U32 nPPL;
    AX_U32 nVNPU;
    AX_BOOL bTrackEnable;

    DETECTOR_CHN_ATTR_S(AX_VOID) {
        nPPL = AX_SKEL_PPL_HVCFP;
        nVNPU = AX_SKEL_NPU_DEFAULT;
        bTrackEnable = AX_FALSE;
    }
} DETECTOR_CHN_ATTR_T;

typedef struct DETECTOR_ATTR_S {
    AX_U32 nGrpCount;
    AX_S32 nSkipRate;
    AX_U32 nW;
    AX_U32 nH;
    AX_S32 nDepth;
    AX_U32 nChannelNum;
    DETECTOR_CHN_ATTR_T tChnAttr[DETECTOR_MAX_CHN_NUM];
    AX_CHAR szModelPath[128];

    DETECTOR_ATTR_S(AX_VOID) {
        nGrpCount = 1;
        nSkipRate = 1;
        nW = 0;
        nH = 0;
        nDepth = 1;
        nChannelNum = 1;
        memset(szModelPath, 0, sizeof(szModelPath));
    }
} DETECTOR_ATTR_T;

typedef struct {
    AX_U64 nSeqNum;
    AX_S32 nGrpId;
    AX_S32 nChnId;
    AX_U32 nSkelChn;
} SKEL_FRAME_PRIVATE_DATA_T;

#ifdef AX_SAMPLE
struct ax_joint_runner_ax650_handle_t
{
    AX_ENGINE_HANDLE handle;
    AX_ENGINE_IO_INFO_T *io_info;
    AX_ENGINE_IO_T io_data;

    unsigned int algo_width, algo_height;
    int algo_colorformat;
};

typedef struct
{
    std::string sName;
    unsigned int nIdx;
    std::vector<unsigned int> vShape;
    int nSize;
    unsigned long phyAddr;
    void *pVirAddr;
} ax_runner_tensor_t;

typedef struct _image_t
{
    unsigned long long int pPhy; // image physical address
    void *pVir;
    unsigned int nSize;
    unsigned int nWidth;
    unsigned int nHeight;
    union
    {
        int tStride_H, tStride_W, tStride_C;
    };
} axdl_image_t;

typedef struct _bbox_t
{
    float x, y, w, h;
} axdl_bbox_t;

typedef struct _results_t
{
    int mModelType; // MODEL_TYPE_E
    int bObjTrack;
    int nObjSize;

    int bPPHumSeg;

    int bYolopv2Mask;

    int nCrowdCount;

    int niFps /*inference*/, noFps /*osd*/;

} axdl_results_t;

typedef struct {
  float x, y, w, h;
} box;

typedef struct {
  box bbox;
  int cls;
  float score;
  int batch_idx;
} detection;
#endif

/**
 * @brief
 *
 */

class CDetector {
public:
    CDetector(AX_VOID) = default;

    AX_BOOL Init(const DETECTOR_ATTR_T& stAttr, AX_BOOL bSimulateDetRets = AX_FALSE);
    AX_BOOL DeInit(AX_VOID);

    AX_BOOL InitSkel();
    AX_BOOL DeInitSkel();

    #ifdef AX_SAMPLE
    AX_BOOL InitEngineHandle();
    AX_BOOL DeInitEngineHandle();
    
    int inference(axdl_image_t *pstFrame, const axdl_bbox_t *crop_resize_box);
    std::vector<detection> post_process(std::vector<detection> dets);
    #endif

    AX_BOOL Start(AX_VOID);
    AX_BOOL Stop(AX_VOID);
    AX_BOOL Clear(AX_VOID);

    AX_BOOL ThreadSwitchAttr(AI_CARD_AI_SWITCH_ATTR_T& tNewAttr);
    AX_BOOL SendFrame(const CAXFrame& axFrame);

    AX_VOID ReleaseSkelPrivateData(SKEL_FRAME_PRIVATE_DATA_T* pData) {
        m_skelData.giveback(pData);
    }

protected:
    AX_BOOL SkipFrame(const CAXFrame& axFrame);
    AX_VOID RunDetect(AX_VOID* pArg);
    AX_VOID ClearQueue(AX_S32 nGrp);
    AX_BOOL SwitchAiAttr();

protected:
    #ifdef AX_SAMPLE
    // detection
    float PROB_THRESHOLD = 0.4f;
    float NMS_THRESHOLD = 0.45f;
    int CLASS_NUM = 80;
    std::vector<float> ANCHORS = {12, 16, 19, 36, 40, 28,
                                  36, 75, 76, 55, 72, 146,
                                  142, 110, 192, 243, 459, 401};
    std::vector<int> STRIDES = {8, 16, 32};
    std::vector<const char*> CLASS_NAMES = {
        "person", "bicycle", "car", "motorcycle", "airplane", "bus", "train", "truck", "boat", "traffic light",
        "fire hydrant", "stop sign", "parking meter", "bench", "bird", "cat", "dog", "horse", "sheep", "cow",
        "elephant", "bear", "zebra", "giraffe", "backpack", "umbrella", "handbag", "tie", "suitcase", "frisbee",
        "skis", "snowboard", "sports ball", "kite", "baseball bat", "baseball glove", "skateboard", "surfboard",
        "tennis racket", "bottle", "wine glass", "cup", "fork", "knife", "spoon", "bowl", "banana", "apple",
        "sandwich", "orange", "broccoli", "carrot", "hot dog", "pizza", "donut", "cake", "chair", "couch",
        "potted plant", "bed", "dining table", "toilet", "tv", "laptop", "mouse", "remote", "keyboard", "cell phone",
        "microwave", "oven", "toaster", "sink", "refrigerator", "book", "clock", "vase", "scissors", "teddy bear",
        "hair drier", "toothbrush"};
    
    struct ax_joint_runner_ax650_handle_t *m_handle = nullptr;
    std::vector<ax_runner_tensor_t> minput_tensors; // handle input tensors
    std::vector<ax_runner_tensor_t> mtensors; // handle result output tensors
    #endif

    CAXLockQ<CAXFrame>* m_arrFrameQ{nullptr};
    DETECTOR_ATTR_T m_stAttr;
    DETECTOR_ATTR_T m_stSwitchingAttr;
    CAXThread m_DetectThread;
    AX_BOOL m_bAiSwitching {AX_FALSE};
    AX_SKEL_HANDLE m_hSkel[DETECTOR_MAX_CHN_NUM]{NULL};
    std::mutex m_mtxSkel;
    CAXResource<SKEL_FRAME_PRIVATE_DATA_T> m_skelData;

    AX_BOOL m_bSimulateDetRets{AX_FALSE};
};
