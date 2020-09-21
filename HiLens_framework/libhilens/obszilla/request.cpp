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

#include "request.h"
#include <cstdio>
#include <regex>
#include <iomanip>
#include <openssl/hmac.h>
#include <openssl/buffer.h>
#include <openssl/md5.h>
#include <curl/curl.h>
#include "obsutils.h"
#include "conf.h"

using namespace std;
using namespace hilens;

RequestURL::RequestURL(const string &url)
{
    // 检查url合法性
    regex re("(https?|ftp|file)://[-A-Za-z0-9+&@#/%?=~_|!:,.;]+[-A-Za-z0-9+&@#/%=~_|]");
    if (!regex_match(url, re)) {
        // 忽略无效的url
        return;
    }

    // 解析URL
    // URL的一般格式为（方括号内为可选项）：
    // protocol://[bucket.]domain[:prot][/object][?param]

    // 获取protocol
    protocol = string(url, 0, url.find_first_of(':'));

    // 获取bucket和domain
    // bucket可以带.甚至可以包含"obs.cn-north-4"这样的字符串，所以不能以.来分割。
    size_t restStrHead = protocol.length() + 3;
    string restStr(&url[restStrHead]);
    // 分出[bucket.]domain
    size_t strBucketDomainTail = restStr.find_first_of(":/"); // 如果没有找到:或者/，就包括整个字符串
    string strBucketDomain = string(restStr, 0, strBucketDomainTail);
    size_t domainHead = strBucketDomain.rfind("obs.");
    if (domainHead == string::npos) {
        // 找不到obs.开头的domain，认定为无效的OBS URL
        return;
    }
    if (domainHead != 0) {
        // domainHead为0说明没有bucket
        bucket = string(strBucketDomain, 0, domainHead - 1); // -1为去掉多余的.号
    }
    domain = string(strBucketDomain, domainHead);

    // 获取port object和param
    if (strBucketDomainTail != string::npos) {
        // 此时restStr包含[:prot][/object][?param]
        restStr = string(restStr, strBucketDomainTail);
        if (restStr[0] == ':') {
            size_t portTail = restStr.find_first_of("/?");
            port = string(restStr, 1, portTail - 1);
            restStr = string(restStr, port.length() + 1);
        }

        if (restStr.empty()) {
            return;
        }

        size_t objectTail = restStr.find_last_of('?');
        if (objectTail != 0) {
            object = string(restStr, 1, objectTail);
        }
        if (objectTail != string::npos) {
            param = string(restStr, objectTail + 1);
        }
    }
}

// URL的一般格式为（方括号内为可选项）：
// protocol://[bucket.]domain[:prot][/object][?param]
string RequestURL::ToString()
{
    string requrl;
    requrl = protocol + "://";
    if (!bucket.empty()) {
        requrl = requrl + bucket + ".";
    }
    requrl = requrl + domain;
    if (!port.empty()) {
        requrl = requrl + ":" + port;
    }
    if (!object.empty()) {
        requrl = requrl + "/" + object;
    }
    if (!param.empty()) {
        requrl = requrl + "?" + param;
    }
    return requrl;
}

const char *Request::MethodStr[4] = {"GET", "POST", "PUT", "HEAD"};

Request::Request(Method method, const RequestURL &url)
{
    write_func = (size_t(*)(const void *, size_t, size_t, void *))fwrite;
    write_userp = nullptr;
    header_func = nullptr;
    header_userp = nullptr;
    read_func = nullptr;
    read_userp = nullptr;
    verbose = 0;
    timeout = 0;

    this->method = method;
    this->url = url;
}

Request::~Request() {}

int Request::Run(long *httpcode)
{
    headers["Date"] = GetDate_RFC1123();
    if (!auth.token.empty()) {
        headers["x-obs-security-token"] = auth.token.c_str();
    }
    if (!auth.ak.empty() && !auth.sk.empty()) {
        string str2sign = StringToSign();
        Sign(str2sign);
    }

    // 添加headers
    struct curl_slist *headerlist = nullptr;
    for (auto it = headers.begin(); it != headers.end(); ++it) {
        headerlist = curl_slist_append(headerlist, string(it->first + ": " + it->second).c_str());
    }

    CURL *handle = curl_easy_init();
    curl_easy_setopt(handle, CURLOPT_VERBOSE, verbose); // 打印详情
    curl_easy_setopt(handle, CURLOPT_CUSTOMREQUEST, MethodStr[method]);
    curl_easy_setopt(handle, CURLOPT_URL, url.ToString().c_str());
    curl_easy_setopt(handle, CURLOPT_HTTPHEADER, headerlist);
    curl_easy_setopt(handle, CURLOPT_CONNECTTIMEOUT, 30L); // 如果30秒都建立不了连接就放弃下载
    curl_easy_setopt(handle, CURLOPT_TIMEOUT, timeout);    // 如果timeout秒都未下载完成也放弃下载

    curl_easy_setopt(handle, CURLOPT_CAINFO, "/etc/ssl/certs/ca-bundle.crt");
    
    if (url.protocol == "https") {
        curl_easy_setopt(handle, CURLOPT_SSLVERSION, CURL_SSLVERSION_TLSv1_2); // 设置TLS版本（公司规定禁用1.0）
        curl_easy_setopt(handle, CURLOPT_USE_SSL, CURLUSESSL_ALL);
    }
    if (write_func) {
        curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, write_func);
        // 如果未提供write_userp则传入stout，否则向NULL写入会导致段错误
        curl_easy_setopt(handle, CURLOPT_WRITEDATA, write_userp ? write_userp : stdout);
    }
    if (header_func) {
        curl_easy_setopt(handle, CURLOPT_HEADERFUNCTION, header_func);
        curl_easy_setopt(handle, CURLOPT_HEADERDATA, header_userp ? header_userp : stdout);
    }
    if (method == POST || method == PUT) {
        curl_easy_setopt(handle, CURLOPT_UPLOAD, 1L);
        curl_easy_setopt(handle, CURLOPT_READFUNCTION, read_func);
        curl_easy_setopt(handle, CURLOPT_READDATA, read_userp);
    } else if (method == HEAD) {
        curl_easy_setopt(handle, CURLOPT_NOBODY, 1L);
    }

    CURLcode ret = curl_easy_perform(handle);
    if (httpcode) {
        curl_easy_getinfo(handle, CURLINFO_RESPONSE_CODE, httpcode);
    }
    curl_easy_cleanup(handle);

    return ret;
}

string Request::StringToSign()
{
    string str2sign;

    str2sign.append(MethodStr[method]).append("\n");

    if (!body.empty()) {
        str2sign.append(MD5ofBody());
    }
    str2sign.append("\n");

    if (headers.find("Content-Type") != headers.end()) {
        str2sign.append(headers["Content-Type"]);
    }
    str2sign.append("\n");

    // 头部如果有x-obs-date项则StringToSign中忽略Date项
    if (headers.find("x-obs-date") == headers.end() && headers.find("Date") != headers.end()) {
        str2sign.append(headers["Date"] + "\n");
    }

    // CanonicalizedHeaders
    for (auto h : headers) {
        // 以x-obs-开头的Header，要求要按字典序（map默认排序是key less，正好符合）
        if (h.first.find("x-obs-") == 0) {
            str2sign.append(h.first + ":" + h.second + "\n");
        }
    }

    // CanonicalizedResource
    str2sign.append("/").append(url.bucket);
    // 桶名和对象名，例如：/bucket/object。如果没有对象名，如列举桶，则为"/bucket/"，如桶名也没有，则为“/”。
    if (!url.object.empty()) {
        str2sign.append("/").append(url.object);
    }
    if (!url.param.empty()) {
        str2sign.append("?").append(url.param);
    }
    return str2sign;
}

int Request::Sign(const string &str2sign)
{
    if (auth.sk.empty()) {
        return SIGN_FAILED;
    }
    // HMAC_SHA1
    static unsigned char hmac[EVP_MAX_MD_SIZE];
    unsigned int hmacLen = 0;
    HMAC_CTX *ctx = HMAC_CTX_new();
    HMAC_CTX_reset(ctx);
    HMAC_Init_ex(ctx, auth.sk.c_str(), auth.sk.length(), EVP_sha1(), NULL);
    HMAC_Update(ctx, (const unsigned char *)str2sign.c_str(), str2sign.length());
    HMAC_Final(ctx, hmac, &hmacLen);

    // BASE64
    BIO *bmem = nullptr;
    BIO *b64 = nullptr;
    BUF_MEM *bptr = nullptr;
    b64 = BIO_new(BIO_f_base64());
    BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);
    bmem = BIO_new(BIO_s_mem());
    b64 = BIO_push(b64, bmem);
    BIO_write(b64, hmac, hmacLen);
    BIO_flush(b64);
    BIO_get_mem_ptr(b64, &bptr);

    string signature(bptr->data, bptr->length);
    headers["Authorization"] = string("OBS ") + auth.ak.c_str() + ":" + signature;

    return OK;
}

string Request::MD5ofBody()
{
    unsigned char md5[MD5_DIGEST_LENGTH];
    // 使用https进行传输可以保证传输通道的机密性和合法性，可以防止中间人攻击，数据的完整性由应用层进行签名来保证。
    // 消费者云服务存储平台使用MD5的场景非公司密码学定义的完整性保护场景，可以作为例外处理。
    MD5((unsigned char *)body.c_str(), body.length(), md5);

    stringstream ss;
    for (int i = 0; i < MD5_DIGEST_LENGTH; ++i) {
        ss << hex << setw(2) << setfill('0') << (int)md5[i];
    }
    return ss.str();
}