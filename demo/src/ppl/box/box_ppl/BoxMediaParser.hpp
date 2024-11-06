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
    AX_U32 nMediaId;
    AX_U32 nMediaDisable;
    AX_U32 nMediaStatus;
    char szMediaUrl[128];
    char szMediaName[32];
    char szMediaDesc[32];
} MEDIA_INFO_T;

class CBoxMediaParser : public CAXSingleton<CBoxMediaParser> {
    friend class CAXSingleton<CBoxMediaParser>;

public:
    std::vector<MEDIA_INFO_T> GetMediasMap(AX_U32 *nCnt, const std::string &strPath);
    AX_BOOL SetMediasMap(std::vector<MEDIA_INFO_T>& vecMedia);

private:
    std::string GetExecPath(AX_VOID);
    std::string m_strPath;

private:
    CBoxMediaParser(AX_VOID) = default;
    ~CBoxMediaParser(AX_VOID) = default;
};
