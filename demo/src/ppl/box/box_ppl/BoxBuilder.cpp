/**************************************************************************************************
 *
 * Copyright (c) 2019-2023 Axera Semiconductor (Shanghai) Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor (Shanghai) Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor (Shanghai) Co., Ltd.
 *
 **************************************************************************************************/

#include "BoxBuilder.hpp"
#include <stdlib.h>
#include <algorithm>
#include "AXPoolManager.hpp"
#include "AppLogApi.h"
#include "DetectObserver.hpp"
#include "DiskHelper.hpp"
#include "DispatchObserver.hpp"
#include "GlobalDef.h"
#include "StreamerFactory.hpp"
#include "VoObserver.hpp"
#include "make_unique.hpp"

#define BOX "BOX"
using namespace std;

#define VDEC_CHN0 0
#define VDEC_CHN1 1
#define VDEC_CHN2 2
#define DISPVO_CHN VDEC_CHN1
#define DETECT_CHN VDEC_CHN2

AX_BOOL CBoxBuilder::Init(AX_VOID) {
    //设置std的warn级别
    //环境变量的优先级更高
    m_sys.InitAppLog("BoxDemo");

    //初始化ini配置
    CBoxConfig *pConfig = CBoxConfig::GetInstance();
    if (!pConfig->Init()) {
        LOG_M_E(BOX, "%s: load box config file fail", __func__);
        return AX_FALSE;
    }

    /* [1]: Load configuration */
    //就是读取ini文件，没有配置的话有默认值
    STREAM_CONFIG_T streamConfig = pConfig->GetStreamConfig();
    DETECT_CONFIG_T detectConfig = pConfig->GetDetectConfig();
    DISPVO_CONFIG_T dispVoConfig = pConfig->GetDispVoConfig("DISPC");
    DISPVO_CONFIG_T dispVoConfig_1 = pConfig->GetDispVoConfig("DISPC1");

    //offline就是因为vpss的数据流要被拿出来编码保存，online模式拿不到
    //默认是false
    if (dispVoConfig.bRecord && dispVoConfig.bOnlineMode) {
        dispVoConfig.bOnlineMode = AX_FALSE;
        LOG_M_C(BOX, ">>>>>>>>>>>>> recorder must set vo to offline mode <<<<<<<<<<<<<<");
    }

    //直接用grp数量确定路数
    //vo的通道数量最大为72，现在为stream num
    m_nDecodeGrpCount = streamConfig.nDecodeGrps;
    if (m_nDecodeGrpCount >= MAX_VO_CHN_NUM) {
        m_nDecodeGrpCount = (dispVoConfig.bShowLogo) ? (MAX_VO_CHN_NUM - 1) : MAX_VO_CHN_NUM;
        LOG_M_E(BOX, "%s: configured %d decoded videos, but %d videos are actived", __func__, streamConfig.nDecodeGrps, m_nDecodeGrpCount);
        streamConfig.nDecodeGrps = m_nDecodeGrpCount;
    }

    if (dispVoConfig.nDevId < 0) {
        LOG_M_E(BOX, "HDMI main device id invalid, please check [DISPC] of box.conf");
        return AX_FALSE;
    }

    //不为空检测disk空间，这个应该是数据流最后保存文件，与vo显示文件记录保存不同
    if (!streamConfig.strSataPath.empty()) {
        if (!CheckDiskSpace(streamConfig)) {
            return AX_FALSE;
        }
    }

    /* [2]: Init system */
    BOX_APP_SYS_ATTR_T tSysAttr{.nMaxGrp = (AX_U32)m_nDecodeGrpCount};
    //系统初始化只需要grp数量,足够解耦合，只需要确定传入的grp数量。
    if (!m_sys.Init(tSysAttr)) {
        return AX_FALSE;
    }

    /* [3]: Init streamers */
    //初始化完系统就初始化流，相当于把流系统初始化好，然后再让输入和输出接入进来
    //对应的结束也是将输入输出关闭，停止数据流之后再把数据流系统关闭
    //其实为什么会那么简单，因为box就是已经确定了业务场景了
    if (!InitStreamer(streamConfig)) {
        return AX_FALSE;
    }

    /* [4]: Init display and observer */
    AX_U32 nVoChn = m_nDecodeGrpCount;
    //disp1 type=1 并且dev还需要不等于-1才显示不一样，相当于由两个vo设备。
    //与channel显示不一样是不同的。
    //确定要有disp1和需要显示异显才分配两组channel
    if (dispVoConfig_1.nDevId != -1 && dispVoConfig_1.nDispType == 1) {
        nVoChn = m_nDecodeGrpCount / 2;
    }

    //这个显示深度与跳帧检测一样，这个地方很奇怪，很可能会导致卡顿的主要原因。
    dispVoConfig.nChnDepth = detectConfig.nSkipRate;
    //一个设备显示多个通道数据，PRIMARY主要的
    //需要注意的是，这里是没有多线程的，是按照channel数量初始化vo硬件的
    if (!InitDisplay(AX_DISPDEV_TYPE::PRIMARY, dispVoConfig, nVoChn)) {
        return AX_FALSE;
    } else {
        /* display recorder */
        //刚刚的是视频流保存路径，这里是画框之后显示的视频流保存
        if (dispVoConfig.bRecord) {
            if (!InitDispRecorder(dispVoConfig.strRecordPath, dispVoConfig.nMaxRecordSize, dispVoConfig.bRecordMuxer)) {
                return AX_FALSE;
            }
        }
    }

    //没有dev1初始化
    dispVoConfig_1.nChnDepth = detectConfig.nSkipRate;

    if (!InitDisplay(AX_DISPDEV_TYPE::SECONDARY, dispVoConfig_1, nVoChn)) {
        return AX_FALSE;
    }

#if 0
    if (dispVoConfig.bOnlineMode || (dispVoConfig_1.nDevId > -1 && dispVoConfig_1.bOnlineMode)) {
        /* fixme: VO online worst cast: keep VB by 2 dispc interrupts */
        if (streamConfig.nChnDepth[DISPVO_CHN] < 6) {
            streamConfig.nChnDepth[DISPVO_CHN] = 6;
        }
    }
#endif

    /* verify */
    for (AX_U32 i = 0; i < m_nDecodeGrpCount; ++i) {
        /* VDEC has no scaler */
        const STREAMER_INFO_T &stream = m_arrStreamer[i]->GetStreamInfo();
        //找到最大值的宽高，也就是不支持视频从小到大，视频需要大于检测的宽高
        AX_U32 nMinW = std::max(detectConfig.nW, m_disp->GetVideoLayout()[0].u32Width);
        AX_U32 nMinH = std::max(detectConfig.nH, m_disp->GetVideoLayout()[0].u32Height);
        if (stream.nWidth < nMinW || stream.nHeight < nMinH) {
            LOG_M_E(BOX, "width %d x height %d of stream < %s > is not supported, please change video which at least > %d x %d",
                    stream.nWidth, stream.nHeight, stream.strPath.c_str(), nMinW, nMinH);
            return AX_FALSE;
        }
    }

    /* [5]: Init detector and observer */
    //初始化算法
    if (detectConfig.bEnable) {
        CDetectResult::GetInstance()->Clear();
        if (!InitDetector(detectConfig)) {
            return AX_FALSE;
        }
    }

    /* [6]: Init dispatchers */
    AX_U32 nDispType = 2;
    if (dispVoConfig_1.nDevId != -1) {
        nDispType = dispVoConfig_1.nDispType;//1
    }

    //默认为0，会进入
    if (!streamConfig.nLinkMode) {
        if (!InitDispatcher(dispVoConfig.strBmpPath, nDispType)) {
            return AX_FALSE;
        }
    } else {
        m_arrDispatcher.clear();
        m_arrDispatchObserver.clear();
    }

    /* [7]: Init video decoder */
    //显示为channel_1,检测为channel_2,为什么会有两个宽高呢？并且显示应该不需要？
    //1920*1080，最终通过ini来配置的宽高
    streamConfig.nChnW[DISPVO_CHN] = m_disp->GetVideoLayout()[0].u32Width;
    streamConfig.nChnH[DISPVO_CHN] = m_disp->GetVideoLayout()[0].u32Height;

    streamConfig.nChnW[DETECT_CHN] = detectConfig.nW;
    streamConfig.nChnH[DETECT_CHN] = detectConfig.nH;
    if (!InitDecoder(streamConfig)) {
        return AX_FALSE;
    }

    for (AX_U32 i = 0; i < m_nDecodeGrpCount; ++i) {
        VDEC_GRP_ATTR_T tGrpAttr;
        m_vdec->GetGrpAttr(i, tGrpAttr);

        //为什么这里还要考虑fps呢？
        AX_U32 nFps = (AX_VDEC_DISPLAY_MODE_PREVIEW == tGrpAttr.eDecodeMode) ? 0 : tGrpAttr.nFps;

        if (m_dispSecondary) {
            if (1 /* DIFF */ == nDispType) {
                //这里设置是帧率，这个也是帧率控制的地方
                if ((i % 2) == 0) {
                    m_disp->SetChnFrameRate(m_disp->GetVideoChn(i / 2), nFps);
                } else {
                    m_dispSecondary->SetChnFrameRate(m_dispSecondary->GetVideoChn(i / 2), nFps);
                }
            } else {
                m_disp->SetChnFrameRate(m_disp->GetVideoChn(i), nFps);
                m_dispSecondary->SetChnFrameRate(m_dispSecondary->GetVideoChn(i), nFps);
            }
        } else {
            m_disp->SetChnFrameRate(m_disp->GetVideoChn(i), nFps);
        }
    }

    /* [8]: vo link vdec */
    //这个地方应该是没有link
    if (streamConfig.nLinkMode) {
        if (dispVoConfig_1.nDevId != -1) {
            //也就是异显
            if (dispVoConfig_1.nDispType == 1) {
                AX_U32 nVoChn = m_nDecodeGrpCount / 2;
                //间接绑定
                // primary
                for (AX_U32 i = 0; i < nVoChn; ++i) {
                    AX_S32 voGrp = (AX_S32)m_disp->GetVideoLayer();
                    AX_S32 voChn = (AX_S32)m_disp->GetVideoChn(i);
                    m_linker.Link({AX_ID_VDEC, (AX_S32)(i * 2), DISPVO_CHN}, {AX_ID_VO, voGrp, voChn});
                }
                // secondary
                for (AX_U32 i = 0; i < nVoChn; ++i) {
                    AX_S32 voGrp = (AX_S32)m_dispSecondary->GetVideoLayer();
                    AX_S32 voChn = (AX_S32)m_dispSecondary->GetVideoChn(i);
                    m_linker.Link({AX_ID_VDEC, (AX_S32)(i * 2 + 1), DISPVO_CHN}, {AX_ID_VO, voGrp, voChn});
                }
            } else if (dispVoConfig_1.nDispType == 0) {
                for (AX_U32 i = 0; i < m_nDecodeGrpCount; ++i) {
                    //分开绑定
                    // primary
                    {
                        AX_S32 voGrp = (AX_S32)m_disp->GetVideoLayer();
                        AX_S32 voChn = (AX_S32)m_disp->GetVideoChn(i);
                        LOG_C("link: vdec vdGrp %d vdChn %d ==> vo voGrp %d voChn %d", i, DISPVO_CHN, voGrp, voChn);
                        m_linker.Link({AX_ID_VDEC, (AX_S32)i, DISPVO_CHN}, {AX_ID_VO, voGrp, voChn});
                    }
                    // secondary
                    {
                        AX_S32 voGrp = (AX_S32)m_dispSecondary->GetVideoLayer();
                        AX_S32 voChn = (AX_S32)m_dispSecondary->GetVideoChn(i);
                        LOG_C("link: vdec vdGrp %d vdChn %d ==> vo voGrp %d voChn %d", i, DISPVO_CHN, voGrp, voChn);
                        m_linker.Link({AX_ID_VDEC, (AX_S32)i, DISPVO_CHN}, {AX_ID_VO, voGrp, voChn});
                    }
                }
            } else {
                LOG_M_E(BOX, "Invalid dual-screen display mode. mode=%d", __func__, dispVoConfig_1.nDispType);
                return AX_FALSE;
            }
        } else {
            for (AX_U32 i = 0; i < m_nDecodeGrpCount; ++i) {
                AX_S32 voGrp = (AX_S32)m_disp->GetVideoLayer();
                AX_S32 voChn = (AX_S32)m_disp->GetVideoChn(i);
                LOG_C("link: vdec vdGrp %d vdChn %d ==> vo voGrp %d voChn %d", i, DISPVO_CHN, voGrp, voChn);
                //link vo和vdec
                m_linker.Link({AX_ID_VDEC, (AX_S32)i, DISPVO_CHN}, {AX_ID_VO, voGrp, voChn});
            }
        }
    }

#if defined(__RECORD_VB_TIMESTAMP__)
    AllocTimestampBufs();
#endif

    return AX_TRUE;
}

//管理n路流
AX_BOOL CBoxBuilder::InitStreamer(const STREAM_CONFIG_T &streamConfig) {
    const AX_U32 nCount = streamConfig.v.size();
    m_arrStreamer.resize(nCount);
    for (AX_U32 i = 0; i < nCount; ++i) {
        STREAMER_ATTR_T stAttr;
        //文件路径
        stAttr.strPath = streamConfig.v[i];
        stAttr.nMaxWidth = streamConfig.nMaxGrpW;
        stAttr.nMaxHeight = streamConfig.nMaxGrpH;
        stAttr.nCookie = (AX_S32)i;
        stAttr.bLoop = AX_TRUE;

        //默认fps
        stAttr.nForceFps = streamConfig.nDefaultFps;
        stAttr.nMaxSendNaluIntervalMilliseconds = CBoxConfig::GetInstance()->GetUTConfig().nMaxSendNaluIntervalMilliseconds;

        //这个地方创建构造函数，流工厂类，根据名字判断的
        m_arrStreamer[i] = CStreamerFactory::GetInstance()->CreateHandler(stAttr.strPath);
        if (!m_arrStreamer[i]) {
            return AX_FALSE;
        }

        //这里就可以用多态的形式进行初始化，而不是使用宏定义的方式进行预编译
        if (!m_arrStreamer[i]->Init(stAttr)) {
            return AX_FALSE;
        }

        LOG_M_C(BOX, "stream %d: %s", i, stAttr.strPath.c_str());
    }

    //流保存初始化
    if (!streamConfig.strSataPath.empty()) {
        m_sataWritter.resize(nCount);
        for (AX_U32 i = 0; i < nCount; ++i) {
            STREAM_RECORD_ATTR_T stAttr = {i, streamConfig.nSataFileSize, streamConfig.nMaxSpaceSize, streamConfig.strSataPath};
            m_sataWritter[i] = make_unique<CStreamRecorder>();
            if (!m_sataWritter[i]) {
                return AX_FALSE;
            }

            //只是分配cache和write buf
            if (!m_sataWritter[i]->Init(stAttr)) {
                return AX_FALSE;
            }

            //每一路都有一个观察者，相当于CStreamRecorder是继承与IstreamObserver
            //流还没开始转动的时候才能绑定观察者
            //数据流绑定了写文件的观察者，类似于回调。
            m_arrStreamer[i]->RegObserver(m_sataWritter[i].get());
        }
    } else {
        m_sataWritter.clear();
    }

    return AX_TRUE;
}

AX_BOOL CBoxBuilder::InitDisplay(AX_DISPDEV_TYPE enDispDev, const DISPVO_CONFIG_T &dispVoConfig, AX_U32 nChnCount) {
    if (dispVoConfig.nDevId == -1) {
        return AX_TRUE;
    }

    if (AX_DISPDEV_TYPE::PRIMARY == enDispDev) {
        m_disp = make_unique<CVo>();
        if (!m_disp) {
            LOG_M_E(BOX, "%s: create display instance fail", __func__);
            return AX_FALSE;
        }
    } else if (AX_DISPDEV_TYPE::SECONDARY == enDispDev) {
        m_dispSecondary = make_unique<CVo>();
        if (!m_dispSecondary) {
            LOG_M_E(BOX, "%s: create secondary display instance fail", __func__);
            return AX_FALSE;
        }
    }

    VO_ATTR_T stAttr;
    stAttr.voDev = dispVoConfig.nDevId;
    //支持hdmi
    stAttr.enIntfType = AX_VO_INTF_HDMI;
    stAttr.enIntfSync = (AX_VO_INTF_SYNC_E)dispVoConfig.nHDMI;
    stAttr.nBgClr = 0x000000;
    if (AX_DISPDEV_TYPE::SECONDARY == enDispDev) stAttr.nBgClr = 0x0000ff;
    //layer深度就是vb 3
    //但是有设置一个channel深度，按照间隔来，可能是为了同步？这个地方问题很大
    stAttr.nLayerDepth = dispVoConfig.nLayerDepth;
    stAttr.nTolerance = dispVoConfig.nTolerance;
    stAttr.strResDirPath = dispVoConfig.strResDirPath;
    stAttr.bShowLogo = dispVoConfig.bShowLogo;
    stAttr.bShowNoVideo = dispVoConfig.bShowNoVideo;
    //确定是onine还是offline，online就是不能保存才对
    stAttr.enMode = (dispVoConfig.bOnlineMode ? AX_VO_MODE_ONLINE : AX_VO_MODE_OFFLINE);
    //初始化32路channel
    stAttr.arrChns.resize(nChnCount);
    for (auto &&m : stAttr.arrChns) {
        m.nPriority = 0;
        //这个深度实际上与检测跳帧是相同的，也就是作为一个缓存，如果检测了才会显示当前帧。
        //举例说，每3帧检测，他vo会缓存3帧，这个意义后面再看，是否会再多分配内存，也是后面再看。
        m.nDepth = dispVoConfig.nChnDepth;
        //并且还是必须有缓存？
        if (m.nDepth < 2) {
            m.nDepth = 2;
        }
    }

    if (AX_DISPDEV_TYPE::PRIMARY == enDispDev) {
        //vo硬件初始化，media类的先不看
        if (!m_disp->Init(stAttr)) {
            return AX_FALSE;
        }
        //这里也只是创建观察者，没有进行绑定，但是这里绑定了chanel 1？这个地方后面要再看看 todo
        //这个地方可能是为了之后保存画框文件，也有可能是绑定vdec
        m_dispObserver = CObserverMaker::CreateObserver<CVoObserver>(m_disp.get(), DISPVO_CHN);
        if (!m_dispObserver) {
            LOG_M_E(BOX, "%s: create display observer instance fail", __func__);
            return AX_FALSE;
        }
    } else if (AX_DISPDEV_TYPE::SECONDARY == enDispDev) {
        if (!m_dispSecondary->Init(stAttr)) {
            return AX_FALSE;
        }
        m_dispObserverSecondary = CObserverMaker::CreateObserver<CVoObserver>(m_dispSecondary.get(), DISPVO_CHN);
        if (!m_dispObserverSecondary) {
            LOG_M_E(BOX, "%s: create display observer instance fail", __func__);
            return AX_FALSE;
        }
    }
    return AX_TRUE;
}

AX_BOOL CBoxBuilder::InitDispRecorder(const string &strRecordPath, AX_S32 nMaxRecordSize, AX_BOOL bMP4) {
    m_dispRecorder = make_unique<CBoxRecorder>();
    if (!m_dispRecorder) {
        LOG_M_E(BOX, "%s: create record instance fail", __func__);
        return AX_FALSE;
    }

    //这里获取编码的配置
    VENC_CONFIG_T vencConfig = CBoxConfig::GetInstance()->GetVencConfig();
    const VO_ATTR_T &tVoAttr = m_disp->GetAttr();
    BOX_RECORDER_ATTR_T conf;
    //终于找到，默认channel 0
    conf.veChn = 0;
    //2
    conf.nFifoDepth[0] = vencConfig.nFifoDepth[0];
    conf.nFifoDepth[1] = vencConfig.nFifoDepth[1];
    conf.nW = tVoAttr.nW;
    conf.nH = tVoAttr.nH;
    conf.nFps = tVoAttr.nHz;
    //为什么都是配置编码的参数
    conf.ePayloadType = (AX_PAYLOAD_TYPE_E)(vencConfig.nPayloadType);
    conf.stRC.eRcType = (AX_VENC_RC_MODE_E)(vencConfig.nRCType);
    conf.stRC.nMinQp = vencConfig.nMinQp;
    conf.stRC.nMaxQp = vencConfig.nMaxQp;
    conf.stRC.nMinIQp = vencConfig.nMinIQp;
    conf.stRC.nMaxIQp = vencConfig.nMaxIQp;
    conf.stRC.nMaxIProp = vencConfig.nMaxIProp;
    conf.stRC.nMinIProp = vencConfig.nMinIProp;
    conf.stRC.nIntraQpDelta = vencConfig.nIntraQpDelta;
    conf.nGop = vencConfig.nGop;
    conf.nBitRate = vencConfig.nBitRate;
    conf.strRecordPath = strRecordPath;
    conf.nMaxRecordSize = ((nMaxRecordSize <= 0) ? 0 : nMaxRecordSize);
    //是否封装成mp4
    conf.bMuxer = bMP4;
    //开启link mode
    conf.bLinkMode = AX_TRUE;
    if (!m_dispRecorder->Init(conf)) {
        return AX_FALSE;
    }

    /* link from vo draw layer to venc */
    //link vo到venc里面，可以理解为进行编码然后保存。
    //而刚刚的是直接保存在文件，相当于原始数据流
    m_linker.Link({AX_ID_VO, (AX_S32)m_disp->GetVideoLayer(), 0}, {AX_ID_VENC, (AX_S32)conf.veChn, 0});
    LOG_M_N(BOX, "enable recording, save path %s", strRecordPath.c_str());

    return AX_TRUE;
}

//在res目录下有默认值
AX_BOOL CBoxBuilder::InitDispatcher(const string &strFontPath, AX_U32 nDispType) {
    //m_arrDispatcher和m_arrDispatchObserver是一个很重要的结构体
    m_arrDispatcher.resize(m_nDecodeGrpCount);
    m_arrDispatchObserver.resize(m_nDecodeGrpCount);
    //每一路都有CDispatcher
    for (AX_U32 i = 0; i < m_nDecodeGrpCount; ++i) {
        m_arrDispatcher[i] = make_unique<CDispatcher>();
        if (!m_arrDispatcher[i]) {
            LOG_M_E(BOX, "%s: create dispatcher %d instance fail", __func__, i);
            return AX_FALSE;
        } else {
            //也就是用 m_arrDispatcher vector 绑定 m_dispObserver，m_dispObserver 在disp中创建
            //不同的就是这里有很多个 disp packer 绑定同一个 m_dispObserver
            if (m_dispObserverSecondary) {
                m_arrDispatcher[i]->RegObserver(m_dispObserverSecondary.get());
            }
            if (m_dispObserver) {
                m_arrDispatcher[i]->RegObserver(m_dispObserver.get());
            }
        }
        //patch从m_dispObserver拿数据

        //先绑定观察者，在初始化
        DISPATCH_ATTR_T stAttr;
        stAttr.vdGrp = i;
        stAttr.strBmpFontPath = strFontPath;
        //深度设置为-1
        stAttr.nDepth = -1;
        stAttr.enDispType = AX_DISP_TYPE(nDispType);
        if (!m_arrDispatcher[i]->Init(stAttr)) {
            return AX_FALSE;
        }

        //而这里只是初始化完 m_arrDispatcher，然后创建用 m_arrDispatchObserver 管理起来，但是只有一个vo，也是channel 1
        m_arrDispatchObserver[i] = CObserverMaker::CreateObserver<CDispatchObserver>(m_arrDispatcher[i].get(), DISPVO_CHN);
        if (!m_arrDispatchObserver[i]) {
            LOG_M_E(BOX, "%s: create dispatch %d observer instance fail", __func__, i);
            return AX_FALSE;
        }
    }

    return AX_TRUE;
}

AX_BOOL CBoxBuilder::InitDetector(const DETECT_CONFIG_T &detectConfig) {
    //创建detector
    m_detect = make_unique<CDetector>();
    if (!m_detect) {
        LOG_M_E(BOX, "%s: create detector instance fail", __func__);
        return AX_FALSE;
    }

    DETECTOR_ATTR_T tDetectAttr;
    //同样这个grp路数很重要
    tDetectAttr.nGrpCount = m_nDecodeGrpCount;
    //间隔检测
    tDetectAttr.nSkipRate = detectConfig.nSkipRate;
    tDetectAttr.nW = detectConfig.nW;
    tDetectAttr.nH = detectConfig.nH;
    //同样buf数量就是grp num*fifo depth 
    tDetectAttr.nDepth = detectConfig.nDepth * m_nDecodeGrpCount;
    tDetectAttr.strModelPath = detectConfig.strModelPath;
    //channel num这个很有意思，而不是根据grp num，而是自定义为3，其实应该设置为grp的，这里是bug？
    //ppl就是选择人车非或者人脸模式，npu就是选择哪个npu推理，track就是是否开启跟踪
    //封装的很好，不得不说。
    tDetectAttr.nChannelNum = AX_MIN(detectConfig.nChannelNum, DETECTOR_MAX_CHN_NUM);
    for (AX_U32 i = 0; i < tDetectAttr.nChannelNum; ++i) {
        tDetectAttr.tChnAttr[i].nPPL = detectConfig.tChnParam[i].nPPL;
        tDetectAttr.tChnAttr[i].nVNPU = detectConfig.tChnParam[i].nVNPU;
        tDetectAttr.tChnAttr[i].bTrackEnable = detectConfig.tChnParam[i].bTrackEnable;
    }
    //然后真正的初始化在这里
    if (!m_detect->Init(tDetectAttr)) {
        return AX_FALSE;
    }
    //同时创建CDetectObserver观察者，但是没做事
    m_detectObserver = CObserverMaker::CreateObserver<CDetectObserver>(m_detect.get(), DETECT_CHN);
    if (!m_detectObserver) {
        LOG_M_E(BOX, "%s: create detect observer fail", __func__);
        return AX_FALSE;
    }

    return AX_TRUE;
}

AX_BOOL CBoxBuilder::InitDecoder(const STREAM_CONFIG_T &streamConfig) {
    m_vdec = make_unique<CVideoDecoder>();
    if (!m_vdec) {
        LOG_M_E(BOX, "%s: create vidoe decoder instance instance fail", __func__);
        return AX_FALSE;
    }

    const COMPRESS_CONFIG_T fbc = CBoxConfig::GetInstance()->GetCompressConfig();
    const DETECT_CONFIG_T &detCfg = CBoxConfig::GetInstance()->GetDetectConfig();

    //根据grp num创建vdec attr
    vector<VDEC_GRP_ATTR_T> arrVdGrps(m_nDecodeGrpCount);
    for (AX_U32 i = 0; i < m_nDecodeGrpCount; ++i) {
        //获取数据流
        const STREAMER_INFO_T &stInfo = m_arrStreamer[i]->GetStreamInfo();

        //下面一直都是在填充这个结构体
        VDEC_GRP_ATTR_T tGrpAttr;
        tGrpAttr.bEnable = AX_TRUE;
        //编码格式h264,并且宽高16对齐
        tGrpAttr.enCodecType = stInfo.eVideoType;
        tGrpAttr.nMaxWidth = ALIGN_UP(streamConfig.nMaxGrpW, 16);  /* H264 MB 16x16 */
        tGrpAttr.nMaxHeight = ALIGN_UP(streamConfig.nMaxGrpH, 16); /* H264 MB 16x16 */

        if (0 == streamConfig.nDefaultFps || STREAM_TYPE_E::RTSP == stInfo.eStreamType) {
            /* if default fps is 0 or RTSP, fps is parsed by streamer */
            //从视频解析
            tGrpAttr.nFps = stInfo.nFps;
        } else {
            /* use configured fps for file streamer */
            //默认值从ini来的
            tGrpAttr.nFps = streamConfig.nDefaultFps;
        }

        //如果是文件类型还可以都环境变量的
        if (STREAM_TYPE_E::FILE == stInfo.eStreamType) {
            /* TODO: debug specified fps for VO module */
            char name[32];
            sprintf(name, "VDEC_GRP%d_DECODED_FPS", i);
            const char *env = getenv(name);
            if (env) {
                tGrpAttr.nFps = atoi(env);
            }
        }

        //解码专用pool
        tGrpAttr.bPrivatePool = (2 == streamConfig.nUserPool) ? AX_TRUE : AX_FALSE;

        if (STREAM_TYPE_E::RTSP == stInfo.eStreamType) {
            /* RTSP: always preview + frame mode */
            tGrpAttr.eDecodeMode = AX_VDEC_DISPLAY_MODE_PREVIEW;
            tGrpAttr.enInputMode = AX_VDEC_INPUT_MODE_FRAME;
            //yuv420
            tGrpAttr.nMaxStreamBufSize = tGrpAttr.nMaxWidth * tGrpAttr.nMaxHeight * 3 / 2;
        } else {
            /* FILE: playback + frame or stream mode according configuration */
            tGrpAttr.eDecodeMode = AX_VDEC_DISPLAY_MODE_PLAYBACK;
            //0表示frame，1表示stream
            if (0 == streamConfig.nInputMode) {
                tGrpAttr.enInputMode = AX_VDEC_INPUT_MODE_FRAME;
                tGrpAttr.nMaxStreamBufSize = tGrpAttr.nMaxWidth * tGrpAttr.nMaxHeight * 3 / 2;
            } else {
                tGrpAttr.enInputMode = AX_VDEC_INPUT_MODE_STREAM;
                tGrpAttr.nMaxStreamBufSize = streamConfig.nMaxStreamBufSize;
            }
        }

        //解码硬件最大的通道数量为3
        for (AX_U32 j = 0; j < MAX_VDEC_CHN_NUM; ++j) {
            //直接用这个引用，就不需要使用指针来指向。
            AX_VDEC_CHN_ATTR_T &tChnAttr = tGrpAttr.stChnAttr[j];
            switch (j) {
                case VDEC_CHN0:
                    //缩放不支持，就不适用channel0?
                    /* pp0 disable, because scaler is not support */
                    tGrpAttr.bChnEnable[j] = AX_FALSE;
                    break;
                case DISPVO_CHN:
                    /* pp1 scaler max. 4096x2160 */
                    tGrpAttr.bChnEnable[j] = AX_TRUE;
                    //通道1的就是stream的宽高
                    tChnAttr.u32PicWidth = streamConfig.nChnW[j];
                    tChnAttr.u32PicHeight = streamConfig.nChnH[j];
                    //vdec需要256对齐
                    tChnAttr.u32FrameStride = ALIGN_UP(tChnAttr.u32PicWidth, VDEC_STRIDE_ALIGN);
                    //如果link，vdec数据不能拿出来画框，直接送到vo，所以不link
                    if (0 == streamConfig.nLinkMode) {
                        //默认为5，vdec需要5个buf
                        tChnAttr.u32OutputFifoDepth = streamConfig.nChnDepth[j];
                    } else {
                        //非link模式为fifo depth
                        tChnAttr.u32OutputFifoDepth = 0;
                    }
                    tChnAttr.enOutputMode = AX_VDEC_OUTPUT_SCALE;
                    //怪不得设置了最大值，这个确定输出为YUV420P，而本身VDEC输出YUV420也是对的
                    tChnAttr.enImgFormat = AX_FORMAT_YUV420_SEMIPLANAR;
                    tChnAttr.stCompressInfo.enCompressMode = (AX_COMPRESS_MODE_E)(fbc.nMode);
                    if (AX_COMPRESS_MODE_LOSSY == fbc.nMode) {
                        tChnAttr.stCompressInfo.u32CompressLevel = fbc.nLv;
                    }
                    break;
                case DETECT_CHN:
                    /* pp2 scaler max. 1920x1080 */
                    tGrpAttr.bChnEnable[j] = (m_detect ? AX_TRUE : AX_FALSE);
                    if (tGrpAttr.bChnEnable[j]) {
                        tChnAttr.u32PicWidth = streamConfig.nChnW[j];
                        tChnAttr.u32PicHeight = streamConfig.nChnH[j];
                        tChnAttr.u32FrameStride = ALIGN_UP(tChnAttr.u32PicWidth, VDEC_STRIDE_ALIGN);
                        tChnAttr.u32OutputFifoDepth = streamConfig.nChnDepth[j];
                        tChnAttr.enOutputMode = AX_VDEC_OUTPUT_SCALE;
                        tChnAttr.enImgFormat = AX_FORMAT_YUV420_SEMIPLANAR;
                        tChnAttr.stCompressInfo.enCompressMode = AX_COMPRESS_MODE_NONE;
#ifdef __VDEC_PP_FRAME_CTRL__
                        if (detCfg.nSkipRate > 1) {
                            tChnAttr.stOutputFrmRate.bFrmRateCtrl = AX_TRUE;
                            tChnAttr.stOutputFrmRate.f32DstFrmRate = stInfo.nFps * 1.0 / detCfg.nSkipRate;
                        }
#endif
                    }
                    break;
                default:
                    break;
            }
        }
        //右值直接复制给arrVdGrps，不需要进行拷贝，每一路都进行配置
        //相当于每一路vdec都配置三个通道的信息
        arrVdGrps[i] = move(tGrpAttr);
    }

    if (!m_vdec->Init(arrVdGrps)) {
        return AX_FALSE;
    }

    //这个相当于n路码流*2路
    for (AX_U32 i = 0; i < m_nDecodeGrpCount; ++i) {
        /* register vdec to streamer */
        m_arrStreamer[i]->RegObserver(m_vdec.get());

        AX_VDEC_GRP vdGrp = (AX_VDEC_GRP)i;

        //这里相当于两路的概念
        //绑定显示观察者，相当于vdec输出给画框rgn模块
        //vdec绑定dispatch，最终调用相应的回调
        if (!streamConfig.nLinkMode) {
            m_vdec->RegObserver(vdGrp, m_arrDispatchObserver[i].get());
        }

        //绑定检测观察者，相当于vdec输出给检测tpu模块
        //同样也是绑定检测的回调
        if (m_detectObserver) {
            m_vdec->RegObserver(vdGrp, m_detectObserver.get());
        }

        VDEC_GRP_ATTR_T tGrpAttr;
        m_vdec->GetGrpAttr(vdGrp, tGrpAttr);
        for (AX_U32 j = 0; j < MAX_VDEC_CHN_NUM; ++j) {
            if (!tGrpAttr.bChnEnable[j]) {
                continue;
            }

            //再次取引用不用拷贝
            AX_VDEC_CHN_ATTR_T &stChn = tGrpAttr.stChnAttr[j];
            //获取blk大小
            AX_U32 nBlkSize = CVideoDecoder::GetBlkSize(stChn.u32PicWidth, stChn.u32PicHeight, VDEC_STRIDE_ALIGN, tGrpAttr.enCodecType,
                                                        &stChn.stCompressInfo, stChn.enImgFormat);

            //也就是他说应该区分I帧和P帧，其实也没关系，因为有grp和channel的概念，怪不得要出两路，一路是ai,一路是rgn+vpss
            if (tGrpAttr.bPrivatePool) {
                /* fixme: we should calculate blksize by SPS such as sps.frame_cropping_flags */
                stChn.u32FrameBufSize = nBlkSize;
                stChn.u32FrameBufCnt = streamConfig.nChnDepth[j];//1：5 2：3
                m_vdec->SetChnAttr(vdGrp, j, stChn);
                continue;
            }

            //果然结合vdec的地方肯定要配合vb，之前只是初始化sys
            if (0 == streamConfig.nUserPool) {
                CAXPoolManager::GetInstance()->AddBlockToFloorPlan(nBlkSize, streamConfig.nChnDepth[j]);
                LOG_M_N(BOX, "VDEC vdGrp %d vdChn %d blkSize %d blkCount %d", vdGrp, j, nBlkSize, streamConfig.nChnDepth[j]);
            } else {
                AX_POOL_CONFIG_T stPoolConfig;
                memset(&stPoolConfig, 0, sizeof(stPoolConfig));
                stPoolConfig.MetaSize = 4096;
                stPoolConfig.BlkSize = nBlkSize;
                //相当于直接预留5个vb，每个grp和channel1以及3个vb在每个grp和channel2
                stPoolConfig.BlkCnt = streamConfig.nChnDepth[j];
                stPoolConfig.IsMergeMode = AX_FALSE;
                stPoolConfig.CacheMode = POOL_CACHE_MODE_NONCACHE;
                sprintf((AX_CHAR *)stPoolConfig.PoolName, "vdec_%d_pp%d_pool", i, j);
                AX_POOL pool = CAXPoolManager::GetInstance()->CreatePool(stPoolConfig);
                if (AX_INVALID_POOLID == pool) {
                    return AX_FALSE;
                }

                //然后绑定vdec和vb
                if (!m_vdec->AttachPool(vdGrp, (AX_VDEC_CHN)j, pool)) {
                    return AX_FALSE;
                }

                LOG_M_C(BOX, "pool %2d (blkSize %d blkCount %d) is attached to VDEC vdGrp %d vdChn %d", pool, stPoolConfig.BlkSize,
                        stPoolConfig.BlkCnt, vdGrp, j);
            }
        }
    }

    //其实只要整体美观，代码就好看
    //设置512个pool
    if (0 == streamConfig.nUserPool) {
        if (!CAXPoolManager::GetInstance()->CreateFloorPlan(512)) {
            return AX_FALSE;
        }
    }

    return AX_TRUE;
}

AX_BOOL CBoxBuilder::DeInit(AX_VOID) {
    /* destory instances */
#define DESTORY_INSTANCE(p) \
    do {                    \
        if (p) {            \
            p->DeInit();    \
            p = nullptr;    \
        }                   \
    } while (0)

    //流
    for (auto &&m : m_arrStreamer) {
        DESTORY_INSTANCE(m);
    }

    //流后面的记录
    for (auto &&m : m_sataWritter) {
        DESTORY_INSTANCE(m);
    }

    //显示叠加
    for (auto &&m : m_arrDispatcher) {
        DESTORY_INSTANCE(m);
    }
    m_arrDispatcher.clear();

    /* If private pool, destory consumer before producer */
    DESTORY_INSTANCE(m_disp);
    DESTORY_INSTANCE(m_dispSecondary);
    DESTORY_INSTANCE(m_detect);
    DESTORY_INSTANCE(m_vdec);

#undef DESTORY_INSTANCE

#if defined(__RECORD_VB_TIMESTAMP__)
    FreeTimestampBufs();
#endif

    m_sys.DeInit();
    m_sys.DeInitAppLog();
    return AX_TRUE;
}

// 最开始入口
AX_BOOL CBoxBuilder::Start(AX_VOID) {
    if (!Init()) {
        DeInit();
        return AX_FALSE;
    }

    do {
        //上面就是初始化，这里就是开始工作。
        //先开启显示记录模块
        if (m_dispRecorder) {
            if (!m_dispRecorder->Start()) {
                return AX_FALSE;
            }
        }

        //再开启显示模块1
        if (m_disp) {
            if (!m_disp->Start()) {
                return AX_FALSE;
            }
        } else {
            LOG_M_E(BOX, "%s: >>>>>>>>>>>>>>>> DISP module is disabled <<<<<<<<<<<<<<<<<<<<<", __func__);
        }

        //再开启显示模块2
        if (m_dispSecondary) {
            if (!m_dispSecondary->Start()) {
                return AX_FALSE;
            }
        }

        //开启检测
        if (m_detect) {
            if (!m_detect->Start()) {
                return AX_FALSE;
            }
        }

        //显示叠加模块，这个是多路
        for (auto &m : m_arrDispatcher) {
            if (!m->Start()) {
                return AX_FALSE;
            }
        }

        //解码模块，数据输入源头
        if (m_vdec) {
            if (!m_vdec->Start()) {
                return AX_FALSE;
            }
        } else {
            LOG_M_E(BOX, "%s: >>>>>>>>>>>>>>>> VDEC module is disabled <<<<<<<<<<<<<<<<<<<<<", __func__);
            return AX_FALSE;
        }

        //记录模块，这个也是多路
        for (auto &&m : m_sataWritter) {
            if (m) {
                if (!m->Start()) {
                    return AX_FALSE;
                }
            }
        }

        //这个是数据流转控制，这个也是多路
        for (auto &&m : m_arrStreamer) {
            if (m) {
                thread t([](IStreamHandler *p) { p->Start(); }, m.get());
                t.detach();
            }
        }

        return AX_TRUE;

    } while (0);

    //先停止所有的流
    StopAllStreams();
    //进行等待
    WaitDone();
    return AX_FALSE;
}

AX_BOOL CBoxBuilder::WaitDone(AX_VOID) {
    STREAM_CONFIG_T streamConfig = CBoxConfig::GetInstance()->GetStreamConfig();
    //先断连所有的
    m_linker.UnlinkAll();

    //先停止所有的
    //记录流也是要遍历的。
    for (auto &&m : m_sataWritter) {
        if (m) {
            m->Stop();
        }
    }

    if (m_detect) {
        m_detect->Stop();
    }

    if (m_dispRecorder) {
        m_dispRecorder->Stop();
    }

    if (m_disp) {
        m_disp->Stop();
    }

    if (m_dispSecondary) {
        m_dispSecondary->Stop();
    }

    //显示肯定需要进行遍历的
    for (auto &&m : m_arrDispatcher) {
        m->Stop();
    }

    if (m_vdec) {
        m_vdec->Stop();
    }

    //再逆初始化
    DeInit();
    return AX_TRUE;
}

AX_BOOL CBoxBuilder::QueryStreamsAllEof(AX_VOID) {
    AX_U32 nEofCnt = 0;

    STREAMER_STAT_T stStat;
    for (auto &&m : m_arrStreamer) {
        if (m) {
            m->QueryStatus(stStat);
            if (!stStat.bStarted) {
                ++nEofCnt;
            }
        } else {
            ++nEofCnt;
        }
    }

    return (nEofCnt >= m_arrStreamer.size()) ? AX_TRUE : AX_FALSE;
}

AX_BOOL CBoxBuilder::StopAllStreams(AX_VOID) {
    AX_U32 nCount = m_arrStreamer.size();
    for (AX_U32 i = 0; i < nCount; ++i) {
        if (m_sataWritter.size() > i && m_sataWritter[i]) {
            m_arrStreamer[i]->UnRegObserver(m_sataWritter[i].get());
        }

        m_arrStreamer[i]->UnRegObserver(m_vdec.get());
    }

    m_vdec->UnRegAllObservers();

    for (auto &&m : m_arrDispatcher) {
        if (m_dispObserver) {
            m->UnRegObserver(m_dispObserver.get());
        }

        if (m_dispObserverSecondary) {
            m->UnRegObserver(m_dispObserverSecondary.get());
        }

        m->Clear();
    }

    if (m_detect) {
        m_detect->Clear();
    }

    vector<thread> v;
    v.reserve(nCount);
    for (auto &&m : m_arrStreamer) {
        if (m) {
            STREAMER_STAT_T stStat;
            if (m->QueryStatus(stStat) && stStat.bStarted) {
                v.emplace_back([](IStreamHandler *p) { p->Stop(); }, m.get());
            }
        }
    }

    for (auto &&m : v) {
        if (m.joinable()) {
            m.join();
        }
    }

    return AX_TRUE;
}

AX_BOOL CBoxBuilder::CheckDiskSpace(const STREAM_CONFIG_T &streamConfig) {
    //v就是stream count
    const AX_U32 TOTAL_STREAM_COUNT = streamConfig.v.size();

    AX_U64 nUsedSpace = {0};
    for (AX_U32 i = 0; i < TOTAL_STREAM_COUNT; ++i) {
        std::string dirPath = GET_SAVE_DIR(streamConfig.strSataPath, i);
        CDiskHelper::CreateDir(dirPath.c_str(), AX_FALSE);
        nUsedSpace += CDiskHelper::GetDirSize(dirPath.c_str());
    }

    //最大空间数量*数量
    AX_U64 nNeedSpace = streamConfig.nMaxSpaceSize * TOTAL_STREAM_COUNT;
    //获取路径下磁盘空间大小
    AX_U64 nFreeSpace = CDiskHelper::GetFreeSpaceSize(streamConfig.strSataPath.c_str());

    LOG_M_C(BOX, "free space %lld(%lld MB), used space %lld(%lld MB), need space %lld(%lld MB)", nFreeSpace, nFreeSpace >> 20, nUsedSpace,
            nUsedSpace >> 20, nNeedSpace, nNeedSpace >> 20);

    //判断空间是否足够
    if ((nFreeSpace + nUsedSpace) < nNeedSpace) {
        LOG_M_E(BOX, "no enough space to save, free %lld(%lld MB) + used space %lld(%lld MB), < %lld(%lld MB)", nFreeSpace,
                nFreeSpace >> 20, nUsedSpace, nUsedSpace >> 20, nNeedSpace, nNeedSpace >> 20);
        return AX_FALSE;
    }

    return AX_TRUE;
}

#if defined(__RECORD_VB_TIMESTAMP__)
AX_VOID CBoxBuilder::AllocTimestampBufs(AX_VOID) {
    m_arrTimestampMods.clear();

    AX_U32 nBufNum = 0;
    const char *env = getenv("TIMESTAMP_APP_BUF_NUM");
    if (env) {
        nBufNum = atoi(env);
    }

    if (0 == nBufNum) {
        return;
    }

    for (AX_U32 i = 0; i < m_nDecodeGrpCount; ++i) {
        VDEC_GRP_ATTR_T stAttr;
        m_vdec->GetGrpAttr(i, stAttr);

        for (AX_U32 j = 0; j < MAX_VDEC_CHN_NUM; ++j) {
            if (!stAttr.bChnEnable[j]) {
                continue;
            }

            AX_MOD_INFO_T m = {AX_ID_USER, (AX_S32)i, (AX_S32)j};
            (AX_VOID) CTimestampHelper::AllocTimestampBuf(m, nBufNum);
            m_arrTimestampMods.push_back(m);
        }
    }
}

AX_VOID CBoxBuilder::FreeTimestampBufs(AX_VOID) {
    for (auto &&m : m_arrTimestampMods) {
        CTimestampHelper::FreeTimestampBuf(m);
    }

    m_arrTimestampMods.clear();
}
#endif