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
#include <string>
#include <vector>
#include "picojson.h"

#include "AXSingleton.h"

typedef struct {
    AX_U32 nModelId;
    char szModelPath[128];
    char szModelName[32];
    char szModelDesc[256];
    char szModelWarning[32];
    char szModelVersion[32];
} MODEL_INFO_T;

class CBoxModelParser : public CAXSingleton<CBoxModelParser> {
    friend class CAXSingleton<CBoxModelParser>;

public:
    std::vector<MODEL_INFO_T> GetModelsMap(AX_U32 *nCnt, const std::string &strPath);
    AX_BOOL SetModelsMap(std::vector<MODEL_INFO_T>& vecMedia);

private:
    std::string m_strPath;

private:
    CBoxModelParser(AX_VOID) = default;
    ~CBoxModelParser(AX_VOID) = default;
};
