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

#ifndef HILENS_INCLUDE_HILENS_H
#define HILENS_INCLUDE_HILENS_H

// 为方便包含，在此头文件中将hilens其他几个头文件包含了
#include "errors.h"
#include "ei_services.h"
#include "log.h"
#include "media_process.h"
#include "model.h"
#include "output.h"
#include "resource.h"
#include "video_capture.h"
#include "audio_capture.h"
#include "audio_output.h"

namespace hilens {
/* *
 * @brief 初始化hilens
 * 在调用hilens的接口之前，须先调用hilens::Init()以全局初始化
 * @param verify 对应console上创建技能的检验值。
 * @return HiLensEC 0为成功，其他为失败
 */
HiLensEC Init(const std::string &verify);

/* *
 * @brief 终止hilens
 * 在程序结束时，须调用hilens::Terminate()以释放相关资源
 *
 * @return HiLensEC 0为成功，其他为失败
 */
HiLensEC Terminate();
} // namespace hilens
#endif // HILENS_INCLUDE_HILENS_H
