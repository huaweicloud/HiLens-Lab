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

#ifndef LIBHILENS_H264_WRITER_H
#define LIBHILENS_H264_WRITER_H

#include <cstdio>
#include "output.h"
#include "vencoder.h"

namespace hilens {
class H264Writer : public Display {
public:
    H264Writer() {}
    virtual ~H264Writer();
    bool Init(const std::string &filepath);
    virtual HiLensEC Show(const cv::Mat &frame);
    VEncoder *encoder = nullptr;
    FILE *fp = nullptr;
    bool encoderInited = false;
};
}
#endif // LIBHILENS_H264_WRITER_H