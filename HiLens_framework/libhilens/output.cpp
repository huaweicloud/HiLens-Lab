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

#include "output.h"

#include <codecvt>
#include <algorithm>

#include <curl/curl.h>
#include <securec.h>
#include <cstdlib>
#include "errors.h"
#include "rtmp_publisher.h"
#include "display_hdmi.h"
#include "h264_writer.h"
#include "obszilla/obszilla.h"
#include "conf.h"
#include "utils.h"
#include "device_utils.h"
#include "sfw_log.h"
#include "apigateway/signer.h"
#include "auth.h"
#include "wsclient.h"

#include "boost/beast/http/status.hpp"

using namespace std;
using namespace boost::beast::http;

namespace hilens {
shared_ptr<Display> Display::Create(Type type, const char *path)
{
    if (type == Type::HDMI) {
        DisplayHDMI *dis = new (nothrow)DisplayHDMI();
        if (!dis) {
            return nullptr;
        }
        return shared_ptr<Display>(dis);
    } else if (type == Type::RTMP) {
        if (!path) {
            ERROR("Display: Unknown Init param! path is not specified.");
            return nullptr;
        }
        RTMPPublisher *dis = new (nothrow)RTMPPublisher();
        if (dis) {
            if (!dis->Init(path)) {
                delete dis;
                return nullptr;
            }
        }
        return shared_ptr<Display>(dis);
    } else if (type == Type::H264_FILE) {
        if (!path) {
            ERROR("Display: Unknown Init param! path is not specified.");
            return nullptr;
        }
        H264Writer *dis = new (nothrow)H264Writer();
        if (dis) {
            if (!dis->Init(path)) {
                delete dis;
                return nullptr;
            }
        }
        return shared_ptr<Display>(dis);
    } else {
        ERROR("Display: Unknown Init param!");
        return nullptr;
    }
}

// 检查上传至OBS的key是否合法
// OBS文件夹命名规则：
// 文件夹名称不能包含以下字符 : \/:*?"<>|。
// 文件夹名称不能以英文句号（.）或斜杠（/）开头或结尾。
// 文件夹的绝对路径总长度不能超过1023字符。
// 任何单个斜杠（/）表示分隔并创建多层级的文件夹。
// 文件目录不能包含//
bool CheckUploadKey(const string &key)
{
    if (key.length() > 1023) {
        ERROR("Invalid key! Remote path is too long (should < 1023)");
        return false;
    }

    if (key[0] == '.' || key[0] == '/') {
        ERROR("Invalid key! Remote path should not start with . or /");
        return false;
    }

    // 检查\:*?"<>|字符
    for (int i = 0; i < key.length(); ++i) {
        char c = key[i];
        if (c == '\\' || c == ':' || c == '*' || c == '?' || c == '\"' || c == '<' || c == '>' || c == '|') {
            ERROR("Invalid key! Remote path should not include \\/:*?\"<>|");
            return false;
        }
    }

    // 检查//
    string::size_type idx = key.find("//");
    if (idx != string::npos) {
        ERROR("Invalid key! Remote path should not contain //");
        return false;
    }

    return true;
}

mutex mtx;

int PrepareUpload(const string &key, const string &mode, OBSZilla &obs, OBSZilla::UploadMode &m, OBSObject &obj)
{
    if (!CheckUploadKey(key)) {
        ERROR("Your key is invalid: %s", key.c_str());
        return INVALID_PARAM;
    }

    obj.bucket = OBSConfig::Instance().Bucket();
    obj.domain = OBSConfig::Instance().Domain();
    obj.object = OBSConfig::Instance().Prefix() + key;

    INFO("Upload file info: bucket=%s, domain=%s, object=%s", obj.bucket.c_str(), obj.domain.c_str(),
        obj.object.c_str());

    if (mode == "write") {
        m = OBSZilla::WRITE;
    } else if (mode == "append") {
        m = OBSZilla::APPEND;
    } else {
        return INVALID_PARAM;
    }
    return OK;
}

HiLensEC UploadFile(const string &key, const string &filepath, const string &mode)
{
#ifdef SF_OFFLINE
    ERROR("Not supported in OFFLINE version.");
    return UNKNOWN_ERROR;
#endif
    OBSZilla obs;
    OBSObject obj;
    OBSZilla::UploadMode m;
    if (OK != PrepareUpload(key, mode, obs, m, obj)) {
        return UNKNOWN_ERROR;
    }
    return obs.Upload(filepath, obj, m);
}

void CXXThreadUploadFileAsync(const string key, const string filepath, const string mode, void (*callback)(int))
{
    int ret = UploadFile(key, filepath, mode);
    mtx.lock();
    if (callback) {
        callback(ret);
    }
    mtx.unlock();
}

HiLensEC UploadFileAsync(const string &key, const string &filepath, const string &mode, void (*callback)(int))
{
    thread t(CXXThreadUploadFileAsync, key, filepath, mode, callback);
    t.detach();
    return OK;
}

HiLensEC UploadBuffer(const string &key, const unsigned char *buffer, size_t bufferSize, const string &mode)
{
#ifdef SF_OFFLINE
    ERROR("Not supported in OFFLINE version.");
    return UNKNOWN_ERROR;
#endif
    OBSZilla obs;
    OBSObject obj;
    OBSZilla::UploadMode m;
    if (OK != PrepareUpload(key, mode, obs, m, obj)) {
        return UNKNOWN_ERROR;
    }
    return obs.UploadBuffer((const char *)buffer, bufferSize, obj, m);
}

void CXXThreadUploadBufferAsync(const string key, shared_ptr<const unsigned char> buffer, size_t bufferSize,
    const string mode, void (*callback)(int))
{
    int ret = UploadBuffer(key, buffer.get(), bufferSize, mode);
    mtx.lock();
    if (callback) {
        callback(ret);
    }
    mtx.unlock();
}

HiLensEC UploadBufferAsync(const string &key, shared_ptr<const unsigned char> buffer, size_t bufferSize,
    const string &mode, void (*callback)(int))
{
    thread t(CXXThreadUploadBufferAsync, key, buffer, bufferSize, mode, callback);
    t.detach();
    return OK;
}

int POST(const string &url, const Json::Value &body, long &httpcode, string *response, POSTHeaders *headers)
{
    // 补充body的公共部分:
    // 在body中添加skill_id和device_id
    Json::Value fixedbody(body);
    if (!fixedbody.isMember("skill_id")) {
        fixedbody["skill_id"] = GetSkillID();
    }

    // 此处deviceid使用云侧给的逻辑id
    if (!fixedbody.isMember("device_id") && Configuration::deviceInfo.Has("device_id")) {
        fixedbody["device_id"] = Configuration::deviceInfo["device_id"];
    }

    // Json转string
    stringstream ss;
    ss << fixedbody;
    string bodystr = ss.str();

    CURL *curl = curl_easy_init();
    CURLcode ret = CURLE_OK;
    // 设置头部
    struct curl_slist *curlHeaders = nullptr;
    curlHeaders = curl_slist_append(curlHeaders, "Content-type: application/json");
    if (headers != NULL) {
        for (auto header : *headers) {
            curlHeaders = curl_slist_append(curlHeaders, header.c_str());
        }
    }
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, curlHeaders);
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "POST");
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_COPYPOSTFIELDS, bodystr.c_str());
    curl_easy_setopt(curl, CURLOPT_NOBODY, 0L);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, CurloptWriteFunction);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, response);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0);
    curl_easy_setopt(curl, CURLOPT_SSLVERSION, CURL_SSLVERSION_TLSv1_2);
    // 设置最多20s超时
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 20L);

    ret = curl_easy_perform(curl);
    if (ret != CURLE_OK) {
        ERROR("curl_easy_perform() failed: %s", curl_easy_strerror(ret));
    } else {
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpcode);
    }
    curl_easy_cleanup(curl);
    return ret;
}

HiLensEC SendMessage_Do(RequestParams &request, const string &url)
{
    status httpcode = status::unknown;
    string response;
    CURL *curl = curl_easy_init();
    CURLcode ret = CURLE_OK;

    // 设置头部
    struct curl_slist *headers = nullptr;
    for (auto header : *request.getHeaders()) {
        std::string headerEntry = header.getKey() + ": " + header.getValue();
        headers = curl_slist_append(headers, headerEntry.c_str());
    }
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, request.getMethod().c_str());
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_COPYPOSTFIELDS, request.getPayload().c_str());
    curl_easy_setopt(curl, CURLOPT_NOBODY, 0L);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, CurloptWriteFunction);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    curl_easy_setopt(curl, CURLOPT_SSLVERSION, CURL_SSLVERSION_TLSv1_2);

    curl_easy_setopt(curl, CURLOPT_CAINFO, "/etc/ssl/certs/ca-bundle.crt");

    // 设置最多30s超时
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30L);

    ret = curl_easy_perform(curl);
    curl_slist_free_all(headers);
    if (ret != CURLE_OK) {
        ERROR("SendMessage: curl_easy_perform ret(%d) failed: %s", ret, curl_easy_strerror(ret));
        curl_easy_cleanup(curl);
        if (CURLE_COULDNT_RESOLVE_HOST == ret) {
            return COULDNT_RESOLVE_HOST;
        } else if (CURLE_WRITE_ERROR == ret) {
            return WRITE_ERROR;
        } else if (CURLE_OPERATION_TIMEDOUT == ret) {
            return TIMEOUT;
        } else {
            return UNKNOWN_ERROR;
        }
    }
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpcode);
    curl_easy_cleanup(curl);

    if ((unsigned)status::ok <= (unsigned)httpcode && (unsigned)status::multiple_choices > (unsigned)httpcode) {
        INFO("Send message success");
        return OK;
    }
    ERROR("Send message failed httpcode(%ld)", httpcode);
    if (status::forbidden == httpcode) {
        return AUTH_FAILED;
    } else if (status::not_found == httpcode) {
        return NOT_FOUND;
    } else if (status::conflict == httpcode) {
        return OBJECT_CONFLICT;
    } else if (status::internal_server_error == httpcode) {
        return SERVER_ERROR;
    } else {
        return UNKNOWN_ERROR;
    }
}

HiLensEC SendMessage(const string &subject, const string &message)
{
    const int SUBJECT_MAX_LENGTH = 170;
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    wstring wsubject = converter.from_bytes(subject.c_str());
    if (subject.empty() || wsubject.size() > SUBJECT_MAX_LENGTH) {
        ERROR("SendMessage: subject illegal");
        return INVALID_PARAM;
    }
    const int MESSAGE_MAX_LENGTH = 85;
    wstring wmessage = converter.from_bytes(message.c_str());
    if (message.empty() || wmessage.size() > MESSAGE_MAX_LENGTH) {
        ERROR("SendMessage: message illegal");
        return INVALID_PARAM;
    }
    // 获取host
    string host = Configuration::api["hilens_host"];
    // 获取project_id
    if (!Configuration::deviceInfo.Has("project_id")) {
        ERROR("SendMessage: Failed to get project id");
        return INTERNAL_ERROR;
    }
    string skillId = GetSkillID();
    if (skillId.empty()) {
        ERROR("SendMessage: skill_id is empty");
        return INTERNAL_ERROR;
    }
    string projectId = Configuration::deviceInfo["project_id"];
    // 拼接URI
    string uri = Configuration::api["license_prefix_v1"] + projectId + "/notification/messages";
    string url = string("https://") + host + uri;

    // 拼接JSON数据
    Json::Value root;
    root["subject"] = subject;
    root["message"] = message;
    root["device_id"] = GetDeviceID();
    root["skill_id"] = skillId;

    stringstream ss;
    ss << root;
    string bodystr = ss.str();

    string param = "";
    RequestParams request("POST", host, uri, param, bodystr);
    Header contentTypeHeader("Content-Type", "application/json");
    request.addHeader(contentTypeHeader);

    // ApiGateway签名
    Hstring ak;
    Hstring sk;
    Hstring token;
#ifndef SF_OFFLINE
    Auth::Instance().Get(ak, sk, token);
    Signer signer(ak.c_str(), sk.c_str());
    signer.createSignature(&request);
    Header projectHeader("X-Project-Id", projectId);
    request.addHeader(projectHeader);
    Header tokenHeader("X-Security-Token", string(token.c_str()));
    request.addHeader(tokenHeader);
#endif

    return SendMessage_Do(request, url);
}


void CXXThreadSendMessageAsync(const string &subject, const string &message, void (*callback)(int))
{
    int ret = SendMessage(subject, message);
    mtx.lock();
    if (callback) {
        callback(ret);
    }
    mtx.unlock();
}

// 发送消息
HiLensEC SendMessageAsync(const string &subject, const string &message, void (*callback)(int))
{
    thread t(CXXThreadSendMessageAsync, subject, message, callback);
    t.detach();
    return OK;
}

HiLensEC SendWSSMessage(const char *msg, int length)
{
#ifndef SF_OFFLINE
    return (HiLensEC)common::WSClient::Instance().SendMessage(msg, length);
#else
    return OK;
#endif
}

void OnWSSMessage(void (*callback)(const char *, int))
{
#ifndef SF_OFFLINE
    common::WSClient::Instance().SetMsgCallback(callback);
#endif
}
} // namespace hilens
