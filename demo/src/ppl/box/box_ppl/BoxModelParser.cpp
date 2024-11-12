// /**************************************************************************************************
//  *
//  * Copyright (c) 2019-2023 Axera Semiconductor (Shanghai) Co., Ltd. All Rights Reserved.
//  *
//  * This source file is the property of Axera Semiconductor (Shanghai) Co., Ltd. and
//  * may not be copied or distributed in any isomorphic form without the prior
//  * written consent of Axera Semiconductor (Shanghai) Co., Ltd.
//  *
//  **************************************************************************************************/
#include "AppLogApi.h"
#include "BoxModelParser.hpp"
#include <fstream>

using namespace std;

#define TAG "PARSER"

template <typename T>
static AX_VOID SET_VALUE(picojson::value &argObj, const string &name, T v)
{
    argObj.get<picojson::object>()[name] = picojson::value(v);
}

template <typename T>
static AX_BOOL GET_VALUE(const picojson::value &argObj, const string &name, T &v)
{
    const picojson::value& obj = argObj.get(name);
    if (obj.is<T>()) {
        v = obj.get<T>();
        return AX_TRUE;
    }
    LOG_M_E(TAG, "get remote device faield <%s> failed.", name);
    return AX_FALSE;
}

std::vector<MODEL_INFO_T> CBoxModelParser::GetModelsMap(AX_U32 *nModelCnt, const std::string &strPath) {

    std::vector<MODEL_INFO_T> vecModel;
    LOG_M_D(TAG, "[%s][%d] +++ ", __func__, __LINE__);

    do {
        m_strPath = std::move(strPath);
        std::ifstream file(m_strPath);
        if (!file.is_open()) {
            LOG_M_E(TAG, "Failed to open json config file: %s", m_strPath.c_str());
            break;
        }

        picojson::value json;
        file >> json;
        string err = picojson::get_last_error();
        if (!err.empty()) {
            LOG_M_E(TAG, "Failed to load json config file: %s", m_strPath.c_str());
            break;
        }

        if (!json.is<picojson::object>()) {
            LOG_M_E(TAG, "Loaded config file is not a well-formatted JSON.");
            break;
        }
        // parse remote device
        else {
            double nCnt = 0;
            if (!GET_VALUE(json, "count", nCnt)) {
                break;
            }

            *nModelCnt = (AX_U32)nCnt;

            const picojson::value& device_list_value = json.get("models");
            if (device_list_value.is<picojson::array>()) {
                const picojson::array& arr_model = device_list_value.get<picojson::array>();
                if (*nModelCnt > arr_model.size()) {
                    LOG_M_W(TAG, "model size is invalid.");
                    *nModelCnt = arr_model.size();
                }

                double dValue = 0.0;
                string strValue = "";
                for (auto mode_value : arr_model) {

                    MODEL_INFO_T modelInfo;
                    if (!GET_VALUE(mode_value, "id", dValue)) break;
                    modelInfo.nModelId = dValue;

                    if (!GET_VALUE(mode_value, "path", strValue)) break;
                    strcpy(modelInfo.szModelPath, strValue.c_str());

                    if (!GET_VALUE(mode_value, "name", strValue)) break;
                    strcpy(modelInfo.szModelName, strValue.c_str());

                    if (!GET_VALUE(mode_value, "desc", strValue)) break;
                    strcpy(modelInfo.szModelDesc, strValue.c_str());

                    if (!GET_VALUE(mode_value, "version", strValue)) break;
                    strcpy(modelInfo.szModelVersion, strValue.c_str());

                    vecModel.emplace_back(modelInfo);
                }
            }
        }
    } while (0);

    LOG_M_D(TAG, "[%s][%d] --- ", __func__, __LINE__);
    return vecModel;
}

AX_BOOL CBoxModelParser::SetModelsMap(std::vector<MODEL_INFO_T>& vecModel) {
    AX_BOOL bRet = AX_FALSE;
    LOG_M_D(TAG, "[%s][%d] +++ ", __func__, __LINE__);

    do {
        picojson::object obj;
        obj["count"] = picojson::value((double)vecModel.size());

        picojson::array arr;
        for (auto &info : vecModel) {
            picojson::object objModel;
            objModel["id"] = picojson::value((double)info.nModelId);
            objModel["path"] = picojson::value(string(info.szModelPath));
            objModel["name"] = picojson::value(string(info.szModelName));
            objModel["desc"] = picojson::value(string(info.szModelDesc));
            objModel["version"] = picojson::value(string(info.szModelVersion));

            arr.push_back(picojson::value(objModel));
        }
        obj["models"] = picojson::value(arr);

        std::ofstream file(m_strPath);
        if (!file.is_open()) {
            LOG_M_E(TAG, "Failed to open json config file: %s", m_strPath.c_str());
            break;
        }
        file << picojson::value(obj).serialize(true);
        string err = picojson::get_last_error();
        if (!err.empty()) {
            LOG_M_E(TAG, "Failed to save json config file: %s", m_strPath.c_str());
            break;
        }

        bRet = AX_TRUE;

    } while (0);

    LOG_M_D(TAG, "[%s][%d] --- ", __func__, __LINE__);
    return bRet;
}
