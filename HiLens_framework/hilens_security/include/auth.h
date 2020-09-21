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

#ifndef HILENS_SECURITY_AUTH_H
#define HILENS_SECURITY_AUTH_H

#ifndef SF_OFFLINE

#include <mutex>
#include <thread>
#include "hstring.h"
#include <string>
#include <atomic>
#include "json/json.h"
#include "check_license.h"

namespace hilens {
/* *
 * 已冻结
 */
#define FREEZE 1
class Auth {
public:
    static Auth &Instance();

    virtual ~Auth();

    void OnTokenQuery(const Json::Value &doc);

    /* *
     * @brief 从agent获取临时ak sk token
     *
     * @param ak 出参，access
     * @param sk 出参，secret
     * @param token 出参，securityTokens
     * @return int 0为成功
     */
    int Get(Hstring &ak, Hstring &sk, Hstring &token);

    /* *
     * @brief 运行认证License的线程
     */
    void RunCheckLicenseThread(const std::string &verify);

    int GetChannelLimit();

    /* *
     * @brief 获取model key
     *
     * @param outModelKey 出参，model key
     * @return int 0为成功
     */
    int GetModelKey(Hstring &outModelKey);

private:
    Auth();
    Auth(const Auth &);

    void RequestAuth();

    /* *
     * @brief 向Console发请求认证技能
     */
    void CheckLicenseThread(const std::string &verify);
    bool RequestLicense(const std::string &verify);

private:
    std::thread checkThread;
    std::atomic_bool keepCheck;

    std::atomic_int licenseValidTime;
    std::atomic_int channelLimit;
    std::atomic_bool checkFlag;

    std::mutex authmtx;
    bool requestTokening;
    Hstring ak;
    Hstring sk;
    Hstring token;
    time_t expiryTime;
    std::string measureType;

    /* *
     * 对应的LG证书
     */
    common::LicenseSignatureStatus licSignStatus;
    Hstring modelKey;
    int freeze;
};
}

#endif // SF_OFFLINE
#endif // HILENS_SECURITY_AUTH_H