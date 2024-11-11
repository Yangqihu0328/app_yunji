/**************************************************************************************************
 *
 * Copyright (c) 2019-2023 Axera Semiconductor (Shanghai) Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor (Shanghai) Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor (Shanghai) Co., Ltd.
 *
 **************************************************************************************************/

#include "BoxAudio.hpp"
#include <string.h>
#include <math.h>
#include "AppLog.hpp"
#include "BoxConfig.hpp"
#include "ElapsedTimer.hpp"
#include "ax_base_type.h"
#include "ax_ivps_api.h"
#include "ax_venc_api.h"
#include "ax_vo_api.h"

#define AUDIO "audio"
using namespace std;

AX_BOOL CAudio::Init() {
    LOG_M_D(AUDIO, "%s: +++", __func__);

    m_stPoolConfig.MetaSize = 8192;
    m_stPoolConfig.BlkSize = 32768;
    m_stPoolConfig.BlkCnt = 64;
    m_stPoolConfig.IsMergeMode = AX_FALSE;
    m_stPoolConfig.CacheMode = POOL_CACHE_MODE_NONCACHE;
    strcpy((char *)m_stPoolConfig.PartitionName, "anonymous");
    snprintf((char *)m_stPoolConfig.PoolName, AX_MAX_POOL_NAME_LEN, "AUDIO_PLAY_%d", 0);

    m_stPoolId = AX_POOL_CreatePool(&m_stPoolConfig);
    if (m_stPoolId == AX_INVALID_POOLID) {
        LOG_M_E(AUDIO, "AX_POOL_CreatePool failed!\n");
        return AX_FALSE;
    }

    LOG_M_D(AUDIO, "%s: ---", __func__);
    return AX_TRUE;
}

AX_BOOL CAudio::DeInit(AX_VOID) {
    LOG_M_D(AUDIO, "%s: +++", __func__);
    AX_S32 ret;

    ret = AX_POOL_DestroyPool(m_stPoolId);
    if (AX_SUCCESS != ret) {
        LOG_M_E(AUDIO, "AX_POOL_DestroyPool failed! Error Code:0x%X\n", ret);
        return AX_FALSE;
    }

    LOG_M_D(AUDIO, "%s: ---", __func__);
    return AX_TRUE;
}

AX_BOOL CAudio::Start(AX_VOID) {
    return AX_TRUE;
}

AX_BOOL CAudio::Stop(AX_VOID) {
    m_PlayThread.Stop();
    m_PlayThread.Join();

    return AX_TRUE;
}

// 数据位宽转换
AX_S32 CAudio::BitsToFormat(AX_U32 bits) {
    switch (bits) {
    case 32:
        m_stAttr.bits = AX_AUDIO_BIT_WIDTH_32;
        break;
    case 24:
        m_stAttr.bits = AX_AUDIO_BIT_WIDTH_24;
        break;
    case 16:
        m_stAttr.bits = AX_AUDIO_BIT_WIDTH_16;
        break;
    default:
        LOG_M_E(AUDIO, "%u bits is not supported.\n", bits);
        return -1;
    }

    return 0;
}

AX_S32 CAudio::xfread(void *ptr, size_t size) {
    std::streamsize sz = 0;

    m_ifs.read((char *)ptr, size);
    sz = m_ifs.gcount();
    if (sz <= 0) {
        LOG_E("Error: fread failed\n");
        return -1;
    }
    return sz;
}

// 解析文件头
AX_S32 CAudio::ParseWaveHeader(uint16_t *bits_per_sample) {
    struct riff_wave_header riff_wave_header;
    struct chunk_header chunk_header;
    struct chunk_fmt chunk_fmt;
    int more_chunks = 1;

    xfread(&riff_wave_header, sizeof(riff_wave_header));
    if ((riff_wave_header.riff_id != ID_RIFF) || (riff_wave_header.wave_id != ID_WAVE)) {
        LOG_E("the file is not a riff/wave file\n");
        m_ifs.close();
        return 1;
    }

    do {
        xfread(&chunk_header, sizeof(chunk_header));

        switch (chunk_header.id) {
        case ID_FMT:
            xfread(&chunk_fmt, sizeof(chunk_fmt));
            /* If the format header is larger, skip the rest */
            if (chunk_header.sz > sizeof(chunk_fmt))
                m_ifs.seekg(chunk_header.sz - sizeof(chunk_fmt), std::ios::cur);
            break;
        case ID_DATA:
            /* Stop looking for chunks */
            more_chunks = 0;
            break;
        default:
            /* Unknown chunk, skip bytes */
            m_ifs.seekg(chunk_header.sz, std::ios::cur);
        }
    } while (more_chunks);

    m_stAttr.channels = chunk_fmt.num_channels;
    m_stAttr.rate = chunk_fmt.sample_rate;
    *bits_per_sample = chunk_fmt.bits_per_sample;

    return 0;
}

// 播放音频
AX_VOID CAudio::SendData(AX_VOID *pArg) {
    AX_S32 ret;
    std::streamsize len = 0;

    // 打开音频文件
    m_ifs.open(m_stAttr.audio_file.c_str(), std::ifstream::binary);
    if (!m_ifs.is_open()) {
        LOG_M_E(AUDIO, "%s: open %s file fail, %s", __func__, m_stAttr.audio_file.c_str(), strerror(errno));
        return;
    }

    uint16_t bits_per_sample;

    // 检测文件的通道数量
    if (ParseWaveHeader(&bits_per_sample)) {
        LOG_M_E(AUDIO, "ParseWaveHeader error\n");
        m_ifs.close();
        return;
    }

    // 重新指定数据位宽
    if (BitsToFormat(bits_per_sample)) {
        m_ifs.close();
        return;
    }

    // 设置属性
    AX_AO_ATTR_T stAttr;
    stAttr.enBitwidth = m_stAttr.bits;
    stAttr.enSoundmode = (m_stAttr.channels == 1 ? AX_AUDIO_SOUND_MODE_MONO : AX_AUDIO_SOUND_MODE_STEREO);
    stAttr.enLinkMode = AX_UNLINK_MODE;
    stAttr.enSamplerate = (AX_AUDIO_SAMPLE_RATE_E)m_stAttr.rate;
    stAttr.U32Depth = 30;
    stAttr.u32PeriodSize = m_stAttr.periodSize;
    stAttr.u32PeriodCount = m_stAttr.periodCount;
    stAttr.bInsertSilence = m_stAttr.insertSilence;
    ret = AX_AO_SetPubAttr(m_stAttr.card, m_stAttr.device, &stAttr);
    if (ret) {
        LOG_M_E(AUDIO, "AX_AI_SetPubAttr failed! ret = %x", ret);
        return;
    }

    // 启用设备
    ret = AX_AO_EnableDev(m_stAttr.card, m_stAttr.device);
    if (ret) {
        LOG_M_E(AUDIO, "AX_AO_EnableDev failed! ret = %x \n", ret);
        return;
    }

    // 设置音量
    ret = AX_AO_SetVqeVolume(m_stAttr.card, m_stAttr.device, m_stAttr.vqeVolume);
    if (ret) {
        LOG_M_E(AUDIO, "AX_AO_SetVqeVolume failed! ret = %x \n", ret);
        AX_AO_DisableDev(m_stAttr.card, m_stAttr.device);
        return;
    }

    AX_U64 BlkSize = 960;
    AX_AUDIO_FRAME_T stFrmInfo;
    memset(&stFrmInfo, 0, sizeof(stFrmInfo));
    stFrmInfo.enBitwidth = m_stAttr.bits;
    stFrmInfo.enSoundmode = (m_stAttr.channels == 1 ? AX_AUDIO_SOUND_MODE_MONO : AX_AUDIO_SOUND_MODE_STEREO);
    while (1) {
        // 申请缓存块
        AX_BLK blkId = AX_POOL_GetBlock(m_stPoolId, BlkSize, NULL);
        if (AX_INVALID_BLOCKID == blkId) {
            LOG_M_E(AUDIO, "%s: AX_POOL_GetBlock(rc logo pool %d blkSize %d) fail", __func__, m_stPoolId, BlkSize);
            return;
        } else {
            stFrmInfo.u32BlkId = blkId;
        }

        // 获取缓存块的虚拟地址
        AX_VOID *vir = AX_POOL_GetBlockVirAddr(blkId);
        if (0 == vir) {
            LOG_M_E(AUDIO, "%s: AX_POOL_GetBlockVirAddr(blkId 0x%x) fail", __func__, blkId);
            AX_POOL_ReleaseBlock(blkId);
            return;
        } else {
            stFrmInfo.u64VirAddr = (AX_U8 *)vir;
        }

        // 读取音频数据
        m_ifs.read((char *)stFrmInfo.u64VirAddr, BlkSize);
        len = m_ifs.gcount();
        if (len > 0) {
            // 发送音频数据
            stFrmInfo.u32Len = len; // 音频数据长度
            ret = AX_AO_SendFrame(m_stAttr.card, m_stAttr.device, &stFrmInfo, -1);
            if (ret != AX_SUCCESS) {
                AX_POOL_ReleaseBlock(stFrmInfo.u32BlkId);
                LOG_M_E(AUDIO, "AX_AO_SendFrame error, ret: %x\n", ret);
                return;
            }
        }
        // 释放缓存块
        AX_POOL_ReleaseBlock(stFrmInfo.u32BlkId);

        if (len <= 0) {
            break;
        }
    }

    // 等待所有数据发送完成
    AX_AO_DEV_STATE_T stStatus;
    while (1) {
        ret = AX_AO_QueryDevStat(m_stAttr.card, m_stAttr.device, &stStatus);
        if (stStatus.u32DevBusyNum == 0) {
            break;
        }
        usleep(10 * 1000);
    }

    // 关闭AO模块
    ret = AX_AO_DisableDev(m_stAttr.card, m_stAttr.device);
    if (ret) {
        LOG_M_E(AUDIO, "AX_AO_DisableDev failed! ret= %x\n", ret);
        return;
    }

    m_ifs.close(); // 关闭文件
}

AX_BOOL CAudio::PlayAudio(std::string file) {
    LOG_M_D(AUDIO, "%s: +++", __func__);

    if (m_PlayThread.IsRunning()) {
        m_PlayThread.Stop();
        m_PlayThread.Join();
    }

    m_stAttr.audio_file = file;

    if (!m_PlayThread.Start([this](AX_VOID *pArg) -> AX_VOID { SendData(pArg); }, this, "PlayAudio", SCHED_FIFO, 99)) {
        LOG_M_E(AUDIO, "%s: create playaudio thread fail", __func__);
        return AX_FALSE;
    }

    LOG_M_D(AUDIO, "%s: ---", __func__);
    return AX_TRUE;
}
