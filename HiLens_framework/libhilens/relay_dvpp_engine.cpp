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

#include "hiai_common.h"
#include "relay_dvpp_engine.h"

// Dest Engine 接收Infer Engine推理输出tensor
HIAI_IMPL_ENGINE_PROCESS("DestDvppEngine", DestDvppEngine, DEST_ENGINE_INPUT_SIZE)
{
    // 接收推理模块输出
    std::shared_ptr<DvppEngineTensorT> output_tensor = std::static_pointer_cast<DvppEngineTensorT>(arg0);

    if (nullptr == output_tensor) {
        return HIAI_ERROR;
    }
    // 直接透传，输出到端口0
    hiai::Engine::SendData(0, "DvppEngineTensorT", std::static_pointer_cast<void>(output_tensor));

    return HIAI_OK;
}