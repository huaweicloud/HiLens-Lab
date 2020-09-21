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

#ifndef HILENS_INCLUDE_RESOURCE_H
#define HILENS_INCLUDE_RESOURCE_H

#include <json/json.h>
#include "errors.h"

namespace hilens {
/* *
 * @brief 返回技能工作区目录的路径
 * @details 由于证书校验等问题，不允许在技能安装目录下写操作，故需要指定各技能可写的工作区位置
 * @return 技能工作区路径（末尾带“/”），如果获取失败则返回空字符串
 */
std::string GetWorkspacePath();

/* *
 * @brief 返回技能模型目录的路径
 * @details 对于技能代码包和模型分离的情况，模型会下载到特定目录，使用此函数来获取该路径
 * @return 技能模型目录的路径（末尾带“/”），如果获取失败则返回空字符串
 */
std::string GetModelDirPath();

/* *
 * @brief 获取技能配置
 * @details 获取技能配置，即技能配置文件中的内容解析成的Json对象（jsoncpp）。
 * 注意此函数每次都会读取配置文件并解析成Json对象，所以如果需要读多个配置项，请将返回值存为一个变量，不要过于频繁的调用GetSkillConfig()
 * @return 技能配置Json对象。如果解析失败，则返回一个空的Json::Value（可用.empty()判断）
 */
Json::Value GetSkillConfig();

/* *
 * @brief 计算一个文件的MD5值
 * @param 文件路径
 * @return 字符串，MD5值。如果读取文件失败，则返回空字符串
 */
std::string MD5ofFile(const std::string &filepath);

/* *
 * @brief 从OBS下载一个文件
 * @param url OBS对象的下载链接
 * @return 0成功，其他为失败
 */
HiLensEC DownloadFileFromOBS(const std::string &url, const std::string &downloadTo);
} // namespace hilens
#endif // HILENS_INCLUDE_RESOURCE_H
