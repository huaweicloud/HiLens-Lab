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

#include <hiaiengine/log.h>
#include <vector>
#include <unistd.h>
#include <thread>
#include <fstream>
#include <algorithm>

#include "encode_src_engine.h"

// Source Engine
HIAI_IMPL_ENGINE_PROCESS("EncodeSrcEngine", EncodeSrcEngine, SOURCE_ENGINE_INPUT_SIZE)
{
    HIAI_StatusT hiai_ret = HIAI_OK;

    // receive data
    if (nullptr == arg0) {
        return HIAI_ERROR;
    }

    // send tata to port 0
    hiai_ret = hiai::Engine::SendData(0, "EncodeEngineTensorT", arg0);
    if (HIAI_OK != hiai_ret) {
        return HIAI_ERROR;
    }
    return HIAI_OK;
}