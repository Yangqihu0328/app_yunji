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
#include "ax_ao_api.h"
#include "ax_base_type.h"
#include "ax_sys_api.h"
#include "ax_global_type.h"
#include "AXThread.hpp"
#include <string>
#include <fstream>

#define ID_RIFF 0x46464952
#define ID_WAVE 0x45564157
#define ID_FMT	0x20746d66
#define ID_DATA 0x61746164

struct riff_wave_header {
    uint32_t riff_id;
    uint32_t riff_sz;
    uint32_t wave_id;
};

struct chunk_header {
    uint32_t id;
    uint32_t sz;
};

struct chunk_fmt {
    uint16_t audio_format;
    uint16_t num_channels;
    uint32_t sample_rate;
    uint32_t byte_rate;
    uint16_t block_align;
    uint16_t bits_per_sample;
};

typedef struct AUDIO_ATTR_T_S {
    AX_U32 card; // 声卡号
    AX_U32 device; // 声卡设备号
    AX_U32 channels; // 通道数
    AX_U32 rate; // 采样率
    AX_AUDIO_BIT_WIDTH_E bits; // 音频数据位宽
    AX_U32 periodSize; // 采样点数量
    AX_U32 periodCount; // 缓存帧数量
    AX_F64 vqeVolume; // 使用原始音量
    AX_BOOL insertSilence; // 是否填充静音数据

    std::string audio_file; // 音频文件

    AUDIO_ATTR_T_S(AX_VOID) {
        card = 0;
        device = 2;
        channels = 2;
        rate = 16000;
        bits = AX_AUDIO_BIT_WIDTH_16;
        periodSize = 160;
        periodCount = 4;
        vqeVolume = 1.0;
        insertSilence = AX_FALSE;
        audio_file = "/opt/data/audio/audio_test.wav";
    }
} AUDIO_ATTR_T;

class CAudio {
public:
    CAudio(AX_VOID) = default;

    AX_BOOL Init();
    AX_BOOL DeInit(AX_VOID);

    AX_BOOL Start(AX_VOID);
    AX_BOOL Stop(AX_VOID);

    AX_BOOL PlayAudio(std::string file);

private:
    AX_S32 BitsToFormat(AX_U32 bits);
    AX_S32 xfread(void *ptr, size_t size);
    AX_S32 ParseWaveHeader(uint16_t *bits_per_sample);
    AX_VOID SendData(AX_VOID *pArg);

protected:
    AX_POOL_CONFIG_T m_stPoolConfig;
    AX_POOL m_stPoolId;
    AUDIO_ATTR_T m_stAttr;
    std::ifstream m_ifs;
    CAXThread m_PlayThread;
};
