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

#ifndef COMMON_UTILS_H
#define COMMON_UTILS_H

#include <json/json.h>
#include <memory>
#include <functional>

namespace hilens {
enum HttpCode {
    HTTP_CODE_OK = 200,
    HTTP_CODE_MULTI = 300,
    HTTP_CODE_UNAUTHORIZED = 401
};

// 字符串转Json对象
bool String2Json(const std::string &str, Json::Value &obj, std::string &errorMsg);

// Json对象转字符串
void Json2String(const Json::Value &obj, std::string &str);

// 读取文件到Json对象
bool File2Json(const std::string &filepath, Json::Value &obj, std::string &errorMsg);

// Json对象保存到文件
bool Json2File(const Json::Value &obj, const std::string &filepath, std::string &errorMsg);

// 一个CURL响应回调函数，可将body写入一个std::string中。userp为该string的指针
size_t CurloptWriteFunction(const void *buffer, size_t size, size_t nmemb, void *userp);

template <typename T> std::shared_ptr<T> MakeSharedArray(size_t size)
{
    // default_delete是STL中的默认删除器
    return std::shared_ptr<T>(new T[size](), std::default_delete<T[]>());
}

template <typename T> std::unique_ptr<T> MakeUniqueArray(size_t size)
{
    // default_delete是STL中的默认删除器
    return std::unique_ptr<T>(new T[size]());
}
// 获取技能ID
const char *GetSkillID();

/* *
 * @brief 执行有返回值的系统命令
 *
 * @param cmd 命令
 * @param result 处理结果
 * @param retval 返回值
 */
void ExecutePopen(const char *cmd, std::string &result, std::string &retval);

/* *
 * @brief 执行系统命令
 *
 * @param cmd 命令
 * @param result 处理结果
 *
 */
void ExecutePopen(const char *cmd, std::string &result);

/* *
 * @brief 执行系统命令
 *
 * @param cmd 命令
 * @param onResponse 响应
 *
 */
void ExecutePopen(const char *cmd, std::function<void(std::string)> onResponse);
} // namespace hilens
#endif // COMMON_UTILS_H