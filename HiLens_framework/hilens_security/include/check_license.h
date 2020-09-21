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

#ifndef HILENS_SECURITY_CHECK_LICENSE_H
#define HILENS_SECURITY_CHECK_LICENSE_H

#ifndef SF_OFFLINE
#include <string>
#include "json/json.h"
namespace common {
typedef enum LICENSE_SIGNATURE_STATUS {
    /* *
     * 验证成功
     */
    LSS_SUCCESS = 0,
    /* *
     * 验证失败
     */
    LSS_FAIL,
} LicenseSignatureStatus;
class CheckLicense {
public:
    CheckLicense();
    ~CheckLicense();

    bool RequestLicense(const std::string &verify, const Json::Value &config, int &licenseValidTime, int &channelLimit,
        int &freeze, LicenseSignatureStatus &licSignStatus, std::string &modelKey);

private:
    bool RequestFromHost(const std::string &verify, const Json::Value &config, int &licenseValidTime, int &channelLimit,
        int &freeze, LicenseSignatureStatus &licSignStatus, std::string &modelKey);
    bool CheckLicenseResult(const std::string &license, int &licenseValidTime, int &channelLimit, int &freeze,
        LicenseSignatureStatus &licSignStatus, std::string &modelKey);
    bool LicenseSignatureVerify(const std::string &licenseSignature);
};
} // namespace common

#endif // SF_OFFLINE
#endif // HILENS_SECURITY_CHECK_LICENSE_H