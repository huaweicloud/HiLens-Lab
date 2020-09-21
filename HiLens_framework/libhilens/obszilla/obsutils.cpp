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

#include "obsutils.h"
#include <unistd.h>
#include <sys/stat.h>
#include <sstream>
#include <iomanip>
#include <openssl/md5.h>
#include <securec.h>

using namespace std;

// 计算哈希时读取文件的batch大小
#define BATCHLEN 65536

string GetDate_RFC1123()
{
    // 获取RFC 1123时间戳
    struct tm *timeinfo = nullptr;
    size_t bufferSize = 80;
    char buffer[bufferSize];
    time_t t = time(nullptr);
    timeinfo = gmtime(&t);
    if (timeinfo == nullptr) {
        return "";
    }
    strftime(buffer, bufferSize, "%a, %d %b %Y %H:%M:%S GMT", timeinfo);
    return buffer;
}

long SizeofFile(const std::string &filepath)
{
    long filesize = -1;
    struct stat statbuff;
    if (stat(filepath.c_str(), &statbuff) < 0) {
        return filesize;
    } else {
        filesize = statbuff.st_size;
    }
    return filesize;
}

size_t UploadBufferReadFunc(void *buffer, size_t size, size_t nmemb, void *userp)
{
    ReadBuf *buf = static_cast<ReadBuf *>(userp);
    size_t cpsize = nmemb < buf->size ? nmemb : buf->size;
    int ret = memcpy_s(buffer, cpsize, buf->data, cpsize);
    if (ret != 0) {
        return 0;
    }
    buf->data += cpsize;
    buf->size -= cpsize;
    return cpsize;
}

size_t ResponseHeaderCallback(const void *buffer, size_t size, size_t nmemb, std::map<std::string, std::string> *userp)
{
    string head((const char *)buffer, nmemb);
    size_t split = head.find_first_of(": ");
    if (split == string::npos) {
        // 不是键值对的先忽略吧
        return nmemb;
    }
    string key = string(head, 0, split);
    string value = string(head, split + 2, head.length() - split - 4); // OBS返回的每项后面还有个\r\n要去掉
    (*userp)[key] = value;
    return nmemb;
}