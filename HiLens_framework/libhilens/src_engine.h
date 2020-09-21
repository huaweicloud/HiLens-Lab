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

#ifndef SRC_ENGINE_H_
#define SRC_ENGINE_H_

#include "hiaiengine/api.h"

#define SOURCE_ENGINE_INPUT_SIZE 1
#define SOURCE_ENGINE_OUTPUT_SIZE 1
using hiai::Engine;

// Source Engine
class SrcEngine : public Engine {
    /* *
     * @ingroup hiaiengine
     * @brief HIAI_DEFINE_PROCESS : 重载Engine Process处理逻辑
     * @[in]: 定义一个输入端口，一个输出端口
     */
    HIAI_DEFINE_PROCESS(SOURCE_ENGINE_INPUT_SIZE, SOURCE_ENGINE_OUTPUT_SIZE);
};

#endif // SRC_ENGINE_H_