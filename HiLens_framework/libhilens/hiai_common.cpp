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

using namespace std;

void GetTransSearPtr(void *inputPtr, std::string &ctrlStr, uint8_t *&dataPtr, uint32_t &dataLen)
{
    DvppEngineTensorT *engine_trans = (DvppEngineTensorT *)inputPtr;
    ctrlStr = std::string((char *)inputPtr, sizeof(DvppEngineTensorT));
    dataPtr = (uint8_t *)engine_trans->data.get();
    dataLen = engine_trans->size;
}

/* *
 * @ingroup hiaiengine
 * @brief GetTransSearPtr,             Deserialization of Trans data
 * @param [in] : ctrl_ptr              Struct Pointer
 * @param [in] : data_ptr              Struct data Pointer
 * @param [out]:std::shared_ptr<void> Pointer to the pointer that is transmitted to the Engine
 * @author w00437212
 */
std::shared_ptr<void> GetTransDearPtr(const char *ctrlPtr, const uint32_t &ctrlLen, const uint8_t *dataPtr,
    const uint32_t &dataLen)
{
    DvppEngineTensorT *engine_trans = (DvppEngineTensorT *)ctrlPtr;
    std::shared_ptr<DvppEngineTensorT> engineTranPtr(new DvppEngineTensorT);

    engineTranPtr->size = engine_trans->size;
    engineTranPtr->codeType = engine_trans->codeType;
    engineTranPtr->data.reset(const_cast<uint8_t *>(dataPtr), hiai::Graph::ReleaseDataBuffer);
    return std::static_pointer_cast<void>(engineTranPtr);
}

// 注册DvppEngineTensorT
HIAI_REGISTER_SERIALIZE_FUNC("DvppEngineTensorT", DvppEngineTensorT, GetTransSearPtr, GetTransDearPtr);

// 注册在engine间传输的几种数据类型
HIAI_REGISTER_DATA_TYPE("InferEngineTensorT", InferEngineTensorT);
HIAI_REGISTER_DATA_TYPE("InferEngineInputT", InferEngineInputT);
HIAI_REGISTER_DATA_TYPE("InferEngineOutputT", InferEngineOutputT);
HIAI_REGISTER_DATA_TYPE("EncodeEngineTensorT", EncodeEngineTensorT);
