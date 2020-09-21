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

#ifndef LIBHILENS_HIAI_COMMON_H
#define LIBHILENS_HIAI_COMMON_H

#include <string>
#include <hiaiengine/api.h>
#include <hiaiengine/data_type.h>
#include <hiaiengine/data_type_reg.h>
#include <hiaiengine/status.h>
#include <hiaiengine/ai_tensor.h>
#include <hiaiengine/init.h>
#include <hiaiengine/ai_memory.h>

#define HIAI_HILENS_ERROR 0x6088

enum {
    HIAI_HILENS_INFO_CODE = 0,
    HIAI_HILENS_DVPP_ERROR_CODE
};

HIAI_DEF_ERROR_CODE(HIAI_HILENS_ERROR, HIAI_INFO, HIAI_HILENS_INFO, "");

HIAI_DEF_ERROR_CODE(HIAI_HILENS_ERROR, HIAI_ERROR, HIAI_HILENS_DVPP_ERROR, "");

enum CodeTypeE {
    CODE_TYPE_H264,
    CODE_TYPE_H265
};

typedef struct DvppEngineTensor {
    int32_t size;
    int32_t codeType;
    std::shared_ptr<u_int8_t> data;

    // 下面serialize函数用于序列化结构体
    template <class Archive> void serialize(Archive &ar)
    {
        ar(size, codeType, data);
    }
} DvppEngineTensorT;

typedef struct DvppEncodeEngineTensor {
    int32_t codeType;
    int32_t width;
    int32_t height;
    int32_t size;
    std::shared_ptr<u_int8_t> data;
} EncodeEngineTensorT;

template <class Archive> void serialize(Archive &ar, EncodeEngineTensorT &data)
{
    ar(data.codeType);
    ar(data.width);
    ar(data.height);
    ar(data.size);

    if (data.size > 0 && data.data.get() == nullptr) {
        data.data.reset(new uint8_t[data.size]);
    }

    ar(cereal::binary_data(data.data.get(), data.size * sizeof(uint8_t)));
}

typedef struct InferEngineTensor {
    int32_t size;
    std::shared_ptr<u_int8_t> data;
} InferEngineTensorT;

template <class Archive> void serialize(Archive &ar, InferEngineTensorT &data)
{
    ar(data.size);
    if (data.size > 0 && data.data.get() == nullptr) {
        data.data.reset(new uint8_t[data.size]);
    }
    ar(cereal::binary_data(data.data.get(), data.size * sizeof(uint8_t)));
}


typedef struct InferEngineInput {
    std::vector<InferEngineTensorT> vec;
} InferEngineInputT;


template <class Archive> void serialize(Archive &ar, InferEngineInputT &data)
{
    ar(data.vec);
}

typedef struct InferEngineOutput {
    std::string errorMsg;
    std::vector<InferEngineTensorT> vec;
} InferEngineOutputT;

template <class Archive> void serialize(Archive &ar, InferEngineOutputT &data)
{
    ar(data.errorMsg, data.vec);
}


inline void DeleteDFree(void *ptr)
{
    if (ptr) { // 以后改接口后注意，so没有引出的函数是不能作为智能指针的析构函数的
        hiai::HIAIMemory::HIAI_DFree(ptr);
    }
}

inline void DeleteNothing(void *ptr) {}

#endif // LIBHILENS_HIAI_COMMON_H