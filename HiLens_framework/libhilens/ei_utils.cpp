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

#include "ei_utils.h"
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include "sfw_log.h"
#include "auth.h"
#include "conf.h"
#include "hstring.h"
#include "apigateway/signer.h"

#define place std::string("ei_utils")

using namespace std;
using namespace hilens;

static const string FACE_VERSION = "v1";
static const string HILENS_VERSION = "v1";

static size_t CurloptWriteFunction(const void *buffer, size_t size, size_t nmemb, void *userp)
{
    if (!userp) {
        return nmemb;
    }
    string *str = static_cast<string *>(userp);
    str->append(string((const char *)buffer, size * nmemb));
    return nmemb;
}

EIUtils &EIUtils::GetInstance()
{
    static EIUtils EI;
    return EI;
}

EIUtils::EIUtils()
{
    // 根据hda.conf中region项来配置调用云侧接口地址中的region
    faceUrl = "face.cn-north-4.myhuaweicloud.com";
    hilensUrl = "hilens-api.cn-north-4.myhuaweicloud.com";
    if (Configuration::hdaConfig["region"] == "cn-north-7") {
        faceUrl = "face.cn-north-7.myhuaweicloud.com";
        hilensUrl = "hilens-api.cn-north-7.myhuaweicloud.com";
    }
    projectID = EIUtils::GetProjectID();
}
EIUtils::~EIUtils() {}

string EIUtils::GetProjectID()
{
    if (!Configuration::deviceInfo.Has("project_id")) {
        ERROR((place + ": Failed to get project id").c_str());
    }
    return Configuration::deviceInfo["project_id"];
}

CURL *EIUtils::CreateCurl(RequestParams &request, Headers &headers, const std::string &fileKey, const string &filePath,
    EIResponse &eiResp, const EIAuth &auth, struct curl_slist *&chunk, curl_mime *&form)
{
    CURL *curl = curl_easy_init();
    if (curl == nullptr) {
        ERROR((place + ": curl_easy_init failed").c_str());
        return nullptr;
    }
    string url = "https://" + request.getHost() + request.getUri() + "?" + request.getQueryParams();

    // 设置头部
    std::set<Header>::iterator it;
    for (auto header : *request.getHeaders()) {
        std::string headerEntry = header.getKey() + ": " + header.getValue();
        chunk = curl_slist_append(chunk, headerEntry.c_str());
    }
    // 添加默认头部X-Security-Token和X-Project-Id
    Hstring tokenHeader = Hstring("X-Security-Token: ") + auth.token;
    string projectHeader = "X-Project-Id: " + projectID;
    chunk = curl_slist_append(chunk, tokenHeader.c_str());
    chunk = curl_slist_append(chunk, projectHeader.c_str());
    // 添加其他头部
    if (headers.empty() == false) {
        for (auto header : headers) {
            chunk = curl_slist_append(chunk, header.c_str());
        }
    }

    curl_mimepart *field = nullptr;
    if (fileKey.empty() && filePath.empty()) {
        curl_easy_setopt(curl, CURLOPT_COPYPOSTFIELDS, request.getPayload().c_str());
        curl_easy_setopt(curl, CURLOPT_NOBODY, 0L);
    } else {
        LOG_DEBUG("start config form data");
        form = curl_mime_init(curl);
        field = curl_mime_addpart(form);
        // 目前MA在线服务只支持单张图片
        curl_mime_name(field, fileKey.c_str());
        curl_mime_filedata(field, filePath.c_str());
        curl_easy_setopt(curl, CURLOPT_MIMEPOST, form);
    }

    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, request.getMethod().c_str());
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, CurloptWriteFunction);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &eiResp.responseBody);
    // SSL校验
    curl_easy_setopt(curl, CURLOPT_SSLVERSION, CURL_SSLVERSION_TLSv1_2); // 设置TLS版本（公司规定禁用1.0）
    curl_easy_setopt(curl, CURLOPT_USE_SSL, CURLUSESSL_ALL);
    curl_easy_setopt(curl, CURLOPT_CAINFO, "/etc/ssl/certs/ca-bundle.crt");
    return curl;
}

EIResponse EIUtils::SendRequest(RequestParams &request, Headers &headers, const std::string &fileKey,
    const string &filePath)
{
    EIResponse eiResp;
    eiResp.requestState = false;
    EIAuth auth;
    CURLcode ret = CURLE_OK;
    long httpcode;

    if (fileKey.empty() && filePath.empty()) {
        // 添加默认header
        request.addHeader("Content-Type", "application/json");
    }

    // ApiGateway签名
#ifndef SF_OFFLINE
    Auth::Instance().Get(auth.ak, auth.sk, auth.token);
    if (auth.ak.empty() || auth.sk.empty()) {
        ERROR((place + ": Failed to get aksk").c_str());
        return eiResp;
    }
    Signer signer(auth.ak.c_str(), auth.sk.c_str());
    signer.createSignature(&request);
#endif
    struct curl_slist *chunk = nullptr;
    curl_mime *form = nullptr;
    CURL *curl = CreateCurl(request, headers, fileKey, filePath, eiResp, auth, chunk, form);
    if (curl == nullptr) {
        return eiResp;
    }

    ret = curl_easy_perform(curl);
    if (ret != CURLE_OK) {
        ERROR("SendRequest: curl_easy_perform() failed: %s\n", curl_easy_strerror(ret));
        curl_slist_free_all(chunk);
        curl_easy_cleanup(curl);
        curl_mime_free(form);
        return eiResp;
    }
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpcode);
    curl_slist_free_all(chunk);
    curl_easy_cleanup(curl);
    curl_mime_free(form);

    if (httpcode >= 200 && httpcode < 300) {
        eiResp.requestState = true;
        return eiResp;
    } else {
        ERROR("SendRequest: request failed, httpcode %ld\n", httpcode);
        return eiResp;
    }
}

EIResponse EIUtils::SendRequest(const std::string &servicesUrl, const std::string &url, const Json::Value &payLoad)
{
    std::string json = payLoad.toStyledString();
    const std::string query = std::string();
    RequestParams request("POST", servicesUrl, url, query, json);
    Headers header;
    return SendRequest(request, header);
}

EIResponse EIUtils::SendDispatherServiceReq(const std::string &serviceName, const std::string &imageBase64)
{
    const std::string url = "/" + HILENS_VERSION + "/" + serviceName;

    Json::Value payLoad;
    payLoad["image_base64"] = imageBase64;

    return SendRequest(hilensUrl, url, payLoad);
}

EIResponse EIUtils::SendRealTimeServiceReq(const std::string &host, const std::string &url, const std::string &fileKey,
    const std::string &filePath)
{
    if (host.empty() || url.empty() || fileKey.empty() || filePath.empty()) {
        ERROR("SendRealTimeServiceReq: request failed, input paramter error.\n");
        EIResponse eiResp;
        eiResp.requestState = false;
        return eiResp;
    } else {
        RequestParams request("POST", host, url, "");
        request.addHeader("X-Sdk-Content-Sha256", "UNSIGNED-PAYLOAD");
        Headers headers;
        return SendRequest(request, headers, fileKey, filePath);
    }
}

EIResponse EIUtils::SearchFace(const std::string &faceSetName, const std::string &imageBase64, int topN,
    double threshold, const std::string &filter)
{
    const std::string service_name = "face-sets";
    const std::string face_sets_service_name = "search";
    const std::string url =
        "/" + FACE_VERSION + "/" + projectID + "/" + service_name + "/" + faceSetName + "/" + face_sets_service_name;

    Json::Value payLoad;
    payLoad["image_base64"] = imageBase64;
    payLoad["top_n"] = topN;
    payLoad["threshold"] = threshold;
    if (filter.empty() == 0) {
        payLoad["filter"] = filter;
    }
    return SendRequest(faceUrl, url, payLoad);
}

EIResponse EIUtils::AddFace(const std::string &faceSetName, const std::string &imageBase64,
    const std::string &externalImageId)
{
    const std::string service_name = "face-sets";
    const std::string face_sets_service_name = "faces";
    const std::string url =
        "/" + FACE_VERSION + "/" + projectID + "/" + service_name + "/" + faceSetName + "/" + face_sets_service_name;

    Json::Value payLoad;
    payLoad["image_base64"] = imageBase64;
    if (externalImageId.empty() == 0) {
        payLoad["external_image_id"] = externalImageId;
    }
    return SendRequest(faceUrl, url, payLoad);
}

EIResponse EIUtils::FaceDetect(const std::string &imageBase64, const std::string &attributes)
{
    const std::string service_name = "face-detect";
    const std::string url = "/" + FACE_VERSION + "/" + projectID + "/" + service_name;

    Json::Value payLoad;
    payLoad["image_base64"] = imageBase64;
    if (!attributes.empty()) {
        payLoad["attributes"] = attributes;
    }
    return SendRequest(faceUrl, url, payLoad);
}

EIResponse EIUtils::FaceCompare(const std::string &base64Image1, const std::string &base64Image2)
{
    const std::string service_name = "face-compare";
    const std::string url = "/" + FACE_VERSION + "/" + projectID + "/" + service_name;

    Json::Value payLoad;
    payLoad["image1_base64"] = base64Image1;
    payLoad["image2_base64"] = base64Image2;
    return SendRequest(faceUrl, url, payLoad);
}

EIResponse EIUtils::LiveDetect(const std::string &video_base64, const std::string &actions,
    const std::string &actiontime)
{
    const std::string service_name = "face-detect";
    const std::string url = "/" + FACE_VERSION + "/" + projectID + "/" + service_name;

    Json::Value payLoad;
    payLoad["video_base64"] = video_base64;
    payLoad["actions"] = actions;
    payLoad["actiontime"] = actiontime;
    return SendRequest(faceUrl, url, payLoad);
}