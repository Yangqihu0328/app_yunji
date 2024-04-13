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
#include <algorithm>
#include "AXSingleton.h"
#include "FileStreamer.hpp"
#ifndef __RTSP_UNSUPPORT__
#include "RtspStreamer.hpp"
#endif
#include "make_unique.hpp"

/**
 * @brief
 *
 */
class CStreamerFactory : public CAXSingleton<CStreamerFactory> {
    friend class CAXSingleton<CStreamerFactory>;

public:
    //传引用
    IStreamerHandlerPtr CreateHandler(const std::string& strPath) noexcept {
        std::string s = strPath;
        //转成小写
        std::transform(s.begin(), s.end(), s.begin(), (int (*)(int))tolower);
        //确定是rtsp流还是文件，这个地方就是
        //也就是Cstreambasehandler是继承与istreamhandle,现在cstream派生出来cfile和crtsp
        //现在返回的是基类指针
        //相当于这个分配内存空间创建，返回值是基类指针类型
        STREAM_TYPE_E eType = (s.find("rtsp:") != std::string::npos) ? STREAM_TYPE_E::RTSP : STREAM_TYPE_E::FILE;
        switch (eType) {
            case STREAM_TYPE_E::FILE:
                return std::make_unique<CFileStreamer>();
#ifndef __RTSP_UNSUPPORT__
            case STREAM_TYPE_E::RTSP:
                return std::make_unique<CRtspStreamer>();
#endif
            default:
                break;
        }

        return nullptr;
    }

private:
    CStreamerFactory(AX_VOID) noexcept = default;
    virtual ~CStreamerFactory(AX_VOID) = default;
};
