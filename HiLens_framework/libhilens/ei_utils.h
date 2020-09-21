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

#ifndef LIBHILENS_EI_UTILS_H
#define LIBHILENS_EI_UTILS_H

#include <curl/curl.h>
#include <string>
#include <map>
#include <functional>
#include "json/json.h"
#include "hstring.h"
#include "apigateway/signer.h"
#include "../include/ei_services.h"

namespace hilens {
struct EIAuth {
    Hstring ak;
    Hstring sk;
    Hstring token;
};

class EIUtils {
public:
    static EIUtils &GetInstance();
    /* *
     * @brief 请求的头部
     * 调用例如：headers.push_back("Content-Type: application/json");然后将其作为POST的参数传入
     */
    using Headers = std::vector<std::string>;

    EIResponse SendRequest(RequestParams &request, Headers &headers, const std::string &fileKey = "",
        const std::string &filePath = "");

    EIResponse SendRequest(const std::string &servicesUrl, const std::string &url, const Json::Value &payLoad);

    EIResponse SendDispatherServiceReq(const std::string &serviceName, const std::string &imageBase64);

    EIResponse SendRealTimeServiceReq(const std::string &host, const std::string &url, const std::string &fileKey,
        const std::string &filePath);

    EIResponse SearchFace(const std::string &faceSetName, const std::string &imageBase64, int topN, double threshold,
        const std::string &filter);

    EIResponse AddFace(const std::string &faceSetName, const std::string &imageBase64,
        const std::string &externalImageId);

    EIResponse FaceDetect(const std::string &imageBase64, const std::string &attributes);

    EIResponse FaceCompare(const std::string &base64Image1, const std::string &base64Image2);
    /* *
     * @brief 活体检测
     * 需要视频输入，暂不对外提供
     */
    EIResponse LiveDetect(const std::string &video_base64, const std::string &actions, const std::string &actiontime);

    ~EIUtils();

private:
    EIUtils();
    std::string GetProjectID();
    CURL *CreateCurl(RequestParams &request, Headers &headers, const std::string &fileKey, const std::string &filePath,
        EIResponse &eiResp, const EIAuth &auth, struct curl_slist *&chunk, curl_mime *&form);
    std::string faceUrl;
    std::string hilensUrl;
    std::string projectID;
};
}
#endif // LIBHILENS_EI_UTILS_H
