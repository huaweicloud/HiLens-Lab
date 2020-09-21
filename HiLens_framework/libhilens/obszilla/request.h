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

#ifndef OBSZILLA_REQUEST_H
#define OBSZILLA_REQUEST_H

#include <string>
#include <map>
#include <functional>
#include "hstring.h"

namespace hilens {
// URL的一般格式为（方括号内为可选项）：
// protocol://[bucket.]domain[:prot][/object][?param]
struct RequestURL {
    std::string protocol;
    std::string bucket;
    std::string domain;
    std::string port;
    std::string object;
    std::string param;
    RequestURL() {}
    // 根据一个url字符串构造
    // 如果url解析失败，其属性将都为空
    RequestURL(const std::string &url);
    // 导出为URL
    std::string ToString();
};

struct OBSAuth {
    Hstring ak;
    Hstring sk;
    Hstring token;
};

class Request {
public:
    enum Method {
        GET,
        POST,
        PUT,
        HEAD
    };
    Request(Method method, const RequestURL &url);
    ~Request();
    // 发起请求
    // httpcode: 出参，响应中的HTTP状态码
    // 返回CURL请求的错误码
    int Run(long *httpcode);
    // 请求头，如: headers["Content-Type"]="text/plain"
    std::map<std::string, std::string> headers;
    // 请求的body
    std::string body;
    // 请求的身份验证
    OBSAuth auth;
    // 响应的写回调，默认为fwrite
    size_t (*write_func)(const void *buffer, size_t size, size_t nmemb, void *userp);
    // 响应的写回调数据指针，如果为回调为fwrite则应当为被写文件的指针
    void *write_userp;
    // 响应的Headers回调，默认为NULL
    size_t (*header_func)(const void *buffer, size_t size, size_t nmemb, void *userp);
    // 响应的Headers回调数据指针，如果为回调为fwrite则应当为被写文件的指针
    void *header_userp;
    // 请求的读回调，默认为NULL
    size_t (*read_func)(void *buffer, size_t size, size_t nmemb, void *userp);
    // 请求的读回调数据指针，如果为回调为fread则应当为被读文件的指针
    void *read_userp;
    // 传输超时设置，当传输时间超过timeout时放弃治疗（毕竟万一网速太差，下几天也不是个办法）
    // 默认为0（永不超时，注意是已经建立连接后传输时的超时设置，内部设置了建立连接的超时限制为30s）
    unsigned int timeout;
    // 是否打印详情到stdout，1为打印，0为不打印。默认为0
    int verbose;
    enum ErrorCode {
        OK = 0,
        SIGN_FAILED
    };

private:
    std::string StringToSign();
    // 添加签名
    int Sign(const std::string &str2sign);
    std::string MD5ofBody();
    static const char *MethodStr[4];
    Method method;
    RequestURL url;
};
}
#endif /* OBSZILLA_REQUEST_H */