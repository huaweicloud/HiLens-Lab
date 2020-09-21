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

#ifndef OBSZILLA_OBSUTILS_H
#define OBSZILLA_OBSUTILS_H

#include <string>
#include <map>

// 获取当前时间戳（RFC1123格式）
std::string GetDate_RFC1123();

// 获取文件大小（字节）。如果获取失败返回-1
long SizeofFile(const std::string &filepath);

// 上传buf时将此结构体传给CURL作为read_func的userp
struct ReadBuf {
    const char *data;
    size_t size;
};

// 用于上传buf的read_func
size_t UploadBufferReadFunc(void *buffer, size_t size, size_t nmemb, void *userp);

// 用于获取响应的headers的回调
size_t ResponseHeaderCallback(const void *buffer, size_t size, size_t nmemb, std::map<std::string, std::string> *userp);

#endif /* OBSZILLA_OBSUTILS_H */