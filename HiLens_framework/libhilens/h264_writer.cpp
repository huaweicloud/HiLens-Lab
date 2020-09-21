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

#include "h264_writer.h"
#include "sfw_log.h"

using namespace hilens;

H264Writer::~H264Writer()
{
    delete encoder;
    if (fp) {
        fclose(fp);
    }
}

bool H264Writer::Init(const std::string &filepath)
{
    encoder = new (std::nothrow) VEncoder();
    if (encoder == nullptr) {
        ERROR("Failed to construct VEncoder");
        return false;
    }

    fp = fopen(filepath.c_str(), "wb");
    if (!fp) {
        ERROR("Failed to open file %s.", filepath.c_str());
        return false;
    }

    return true;
}

HiLensEC H264Writer::Show(const cv::Mat &frame)
{
    if (!encoderInited) {
        if (encoder->Init(frame.cols, frame.rows * 2 / 3) != 0) {
            ERROR("Failed to init encoder");
            return UNKNOWN_ERROR;
        }
        encoderInited = true;
    }

    // 编码，如果编码失败则忽略该帧
    unsigned char *framedata = nullptr; // h264帧数据块
    unsigned int framesize = 0;         // h264帧数据块大小
    if (encoder->EncodeFrame(frame.data, frame.total() * frame.elemSize(), &framedata, &framesize) != 0) {
        WARN("Failed to encode frame.");
        return INTERNAL_ERROR;
    }

    fwrite(framedata, 1, framesize, fp);
    free(framedata);
    framedata = nullptr;

    return OK;
}