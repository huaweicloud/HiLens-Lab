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

#include "obszilla.h"
#include <cstdio>
#include <sys/stat.h>
#include <curl/curl.h>
#include "obsutils.h"
#include "sfw_log.h"
#include "auth.h"

using namespace std;
using namespace hilens;

map<int, HiLensEC> OBSZilla::reqError;
map<int, HiLensEC> OBSZilla::httpError;

const int HTTP_OK = 200;
const int HTTP_AUTH_FAILED = 403;
const int HTTP_NOT_FOUND = 404;
const int HTTP_OBJECT_CONFLICT = 409;
const int HTTP_SERVER_ERROR = 500;
const int MKDIR_MODE = 0700;

HiLensEC OBSZilla::GlobalInit()
{
    if (curl_global_init(CURL_GLOBAL_ALL) != CURLE_OK) {
        return INIT_CURL_ERROR;
    }

    // 填充错误码
    reqError[CURLE_OK] = OK;
    reqError[CURLE_COULDNT_RESOLVE_HOST] = COULDNT_RESOLVE_HOST;
    reqError[CURLE_WRITE_ERROR] = WRITE_ERROR;
    reqError[CURLE_OPERATION_TIMEDOUT] = TIMEOUT;

    httpError[HTTP_OK] = OK;
    httpError[HTTP_AUTH_FAILED] = AUTH_FAILED;
    httpError[HTTP_NOT_FOUND] = NOT_FOUND;
    httpError[HTTP_OBJECT_CONFLICT] = OBJECT_CONFLICT;
    httpError[HTTP_SERVER_ERROR] = SERVER_ERROR;

    return OK;
}

OBSZilla::OBSZilla()
{
    timeout = 0;
    verbose = 0;
}

OBSZilla::~OBSZilla() {}

HiLensEC OBSZilla::Download(const RequestURL &url)
{
    // 解析文件名
    string filename = string(url.object, url.object.find_last_of('/') + 1);

    // 检查downloadTo目录
    char resolvedPath[PATH_MAX];
    if (realpath(downloadTo.c_str(), resolvedPath) == NULL && Mkdirs(downloadTo) != 0) {
        ERROR("OBSZilla::Download: Failed to mkdirs!");
        return CREATE_DIR_FAILED;
    }

    if (realpath(downloadTo.c_str(), resolvedPath) == NULL) {
        ERROR("OBSZilla::Download: Failed to mkdirs!");
        return CREATE_DIR_FAILED;
    }
    string filepath = string(resolvedPath) + "/" + filename;

    // 打开文件
    FILE *fp = nullptr;
    if (!(fp = fopen(filepath.c_str(), "wb"))) {
        return OPENFILE_FAILED;
    }

    // 发起请求
    long httpcode = 200;
    Request req(Request::GET, url);
    req.auth = GetAuth();
    req.timeout = timeout;
    req.write_userp = fp;
    req.verbose = verbose;
    int ret = req.Run(&httpcode);

    // 关闭文件
    fclose(fp);

    // 返回错误码
    HiLensEC rc = GetErrorCode(ret, httpcode);
    return rc;
}

HiLensEC OBSZilla::Download(const char *url)
{
    return Download(string(url));
}

HiLensEC OBSZilla::Download(const OBSObject &obj)
{
    RequestURL url;
    url.protocol = "https";
    url.bucket = obj.bucket;
    url.domain = obj.domain;
    url.object = obj.object;
    return Download(url);
}

int OBSZilla::Mkdirs(const string &FileDir)
{
    size_t pos = string::npos;
    string fileDir = "";
    string fileDirTmp = FileDir;
    while (true) {
        if ((pos = fileDirTmp.find_first_of("/")) != string::npos) {
            if (pos == 0) {
                fileDir.append("/");
                fileDirTmp = fileDirTmp.substr(pos + 1);
            } else {
                fileDir.append(fileDirTmp.substr(0, pos));
                fileDirTmp = fileDirTmp.substr(pos);
            }
            mkdir(fileDir.c_str(), (mode_t)MKDIR_MODE);
            continue;
        } else {
            fileDir.append(fileDirTmp);
            mkdir(fileDir.c_str(), (mode_t)MKDIR_MODE);
            break;
        }
    }
    return 0;
}

HiLensEC OBSZilla::Upload(const std::string &filepath, const OBSObject &obj, const UploadMode mode)
{
    // 检查filepath
    char resolvedPath[PATH_MAX];
    if (realpath(filepath.c_str(), resolvedPath) == NULL) {
        ERROR("OBSZilla::Upload: filepath invalid!");
        return ACCESS_FILE_FAILED;
    }
    string rpath(resolvedPath);

    long fileSize = SizeofFile(rpath);
    if (fileSize == -1) {
        ERROR("OBSZilla::Upload: file not exist!");
        return ACCESS_FILE_FAILED;
    }

    // 打开文件
    FILE *fp = nullptr;
    if (!(fp = fopen(rpath.c_str(), "rb"))) {
        ERROR("OBSZilla::Upload: open file failed!");
        return OPENFILE_FAILED;
    }

    // 发起请求
    RequestURL url;
    url.protocol = "https";
    url.bucket = obj.bucket;
    url.domain = obj.domain;
    url.object = obj.object;
    if (mode == APPEND) {
        // 追加模式添加追加参数
        string position = GetObjectNextAppendPos(obj);
        if (position.empty()) {
            fclose(fp);
            ERROR("OBSZilla::Upload: UploadMode error, failed to get object append position!");
            return APPEND_FAILED; // 获取追加位置失败
        }
        url.param = "append&position=" + position;
    }

    // 必须为long，否则踩内存
    long httpcode = 200;
    Request::Method method = mode == WRITE ? Request::PUT : Request::POST;
    Request req(method, url);
    req.auth = GetAuth();
    req.timeout = timeout;
    req.read_userp = fp;
    req.verbose = verbose;
    req.headers["Content-Length"] = to_string(fileSize);
    int ret = req.Run(&httpcode);

    // 关闭文件
    fclose(fp);

    // 返回错误码
    return GetErrorCode(ret, httpcode);
}

HiLensEC OBSZilla::UploadBuffer(const char *buf, size_t bufSize, const OBSObject &obj, const UploadMode mode)
{
    // 检查buf
    if (!buf || !bufSize) {
        return INVALID_BUF;
    }

    ReadBuf userBuf;
    userBuf.data = buf;
    userBuf.size = bufSize;

    // 发起请求
    RequestURL url;
    url.protocol = "https";
    url.bucket = obj.bucket;
    url.domain = obj.domain;
    url.object = obj.object;
    if (mode == APPEND) {
        // 追加模式添加追加参数
        string position = GetObjectNextAppendPos(obj);
        if (position.empty()) {
            ERROR("OBSZilla::UploadBuffer: UploadMode error, failed to get object append position!");
            return APPEND_FAILED; // 获取追加位置失败
        }
        url.param = "append&position=" + position;
    }

    // 必须为long，否则踩内存
    long httpcode = 200;
    Request::Method method = mode == WRITE ? Request::PUT : Request::POST;
    Request req(method, url);
    req.auth = GetAuth();
    req.timeout = timeout;
    req.read_func = UploadBufferReadFunc;
    req.read_userp = &userBuf;
    req.verbose = verbose;
    req.headers["Content-Length"] = to_string(bufSize);
    int ret = req.Run(&httpcode);

    // 返回错误码
    return GetErrorCode(ret, httpcode);
}

HiLensEC OBSZilla::GetErrorCode(int ret, int httpcode)
{
    INFO("OBSZilla: CURL code = %d, HTTP status = %d", ret, httpcode);

    if (ret == CURLE_OK) {
        // CURL请求成功，看httpcode是否为200
        return httpError.find(httpcode) == httpError.end() ? UNKNOWN_ERROR : httpError[httpcode];
    } else {
        // CURL请求失败
        return reqError.find(ret) == reqError.end() ? UNKNOWN_ERROR : reqError[ret];
    }
}

string OBSZilla::GetObjectNextAppendPos(const OBSObject &obj)
{
    // 发起请求
    RequestURL url;
    url.protocol = "https";
    url.bucket = obj.bucket;
    url.domain = obj.domain;
    url.object = obj.object;

    long httpcode = 200;
    map<string, string> resHeaders;
    Request req(Request::HEAD, url);
    req.auth = GetAuth();
    req.header_func = (size_t(*)(const void *, size_t, size_t, void *))ResponseHeaderCallback;
    req.header_userp = &resHeaders;
    req.timeout = timeout;
    req.verbose = verbose;
    int ret = req.Run(&httpcode);

    int rc = GetErrorCode(ret, httpcode);
    if (rc == NOT_FOUND) { // 无此对象则定next append pos为0
        return "0";
    } else if (rc == OK) {
        if (resHeaders.find("x-obs-next-append-position") == resHeaders.end()) {
            // 不具有追加属性的对象不可追加
            return "";
        }
        return resHeaders["x-obs-next-append-position"];
    }
    // 获取追加属性失败
    return "";
}

OBSAuth OBSZilla::GetAuth()
{
    OBSAuth auth;
#ifndef SF_OFFLINE
    Auth::Instance().Get(auth.ak, auth.sk, auth.token);
#endif
    return auth;
}