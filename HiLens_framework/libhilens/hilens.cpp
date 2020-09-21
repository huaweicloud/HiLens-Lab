/* *
 * Copyright 2020 Huawei Technologies Co., Ltd
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "hilens.h"
#include <log4cplus/log4cplus.h>
#include "sfw_log.h"
#include "obszilla/obszilla.h"
#include "hiai_common.h"
#include "hilens_security.h"
#include "wsclient.h"
#include <regex>
#include "conf.h"
#include "auth.h"

namespace hilens {
#define ASSERT_OK(ec) if ((ec) != OK) { \
        return (ec);  \
    }

extern HiLensEC InitSkillLogger();

HiLensEC Init(const std::string &verify)
{
    // 加载配置
    Configuration::Init();

    // 初始化日志
    log4cplus::initialize();
    HiLensEC ec = OK;
    ec = InitSkillLogger();
    ASSERT_OK(ec)

    ec = InitSFWLogger();
    ASSERT_OK(ec)

    // 检查字符串的合法性
    const std::regex re("^[a-z0-9A-Z]+$");
    if (!std::regex_match(verify, re)) {
        ERROR("Invalid skill code, please check the verify string!");
        return INVALID_PARAM;
    }

    // 初始化HiAiEngine
    HIAI_Init(0);

#ifndef SF_OFFLINE
    // 初始化HilensSecurity实例，必须提前初始化，否则在hilens_media起来之后初始化HilensSecurity会导致1004错误
    HilensSecurity::Instance().Init();

    // 初始化OBSZilla
    ec = OBSZilla::GlobalInit();
    ASSERT_OK(ec);
    // 加载摄像头配置
    DeviceConfig::Init();
    // 加载OBS设置
    OBSConfig::Instance().Init();
    // 初始化并连接到hdad
    const string LOCAL_HOST = "127.0.0.1";
    if ((ec = (HiLensEC)common::WSClient::Instance().Init(LOCAL_HOST.c_str(), GetSkillID())) != OK) {
        Error("WSClient init fail");
        return ec;
    }
    if ((ec = (HiLensEC)common::WSClient::Instance().ConnectAsync()) != OK) {
        Error("WSClient connect fail");
        return ec;
    }
    // 检查License
    Auth::Instance().RunCheckLicenseThread(verify);
#endif

    return OK;
}

HiLensEC Terminate()
{
    log4cplus::Logger::shutdown();

    return OK;
}
} // namespace hilens
