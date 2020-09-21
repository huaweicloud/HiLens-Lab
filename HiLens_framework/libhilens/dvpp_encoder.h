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

#ifndef LIBHILENS_DVPP_ENCODER_H
#define LIBHILENS_DVPP_ENCODER_H

#ifdef CLOUD

#include <memory>
#include <queue>
#include <condition_variable>

#include "model.h"
#include "hiai_common.h"

namespace hilens {
class DvppEncoder;

// 回调函数
class DvppEncodeRecvInterface : public hiai::DataRecvInterface {
public:
    DvppEncodeRecvInterface();
    ~DvppEncodeRecvInterface();
    HIAI_StatusT RecvData(const std::shared_ptr<void> &message);
    void SetDepend(DvppEncoder *dependDvppImpl);

    bool flag; // 是否接收完成
    std::shared_ptr<DvppEngineTensorT> output;
    DvppEncoder *dependDvppImpl;
};

class DvppEncoder {
public:
    DvppEncoder(const std::string &filepath) : framedata(new DvppEngineTensorT)
    {
        if (filepath == "h264") {
            codeType = CODE_TYPE_H264;
        } else {
            codeType = CODE_TYPE_H265;
        }
    }
    virtual ~DvppEncoder();

    bool Init(int width, int height);
    virtual HiLensEC Encode(const InferData &inputs);
    virtual HiLensEC Encode(const unsigned char *data, const int size);
    void AddData(const DvppEngineTensorT &data);
    void ReadData(InferData &data);
    int GetBuffSize();

private:
    int SetCallback();

private:
    const int QUEUE_NUM = 10;
    const int FRAME_BUFF_SIZE = 4 * 1024 * 1024; // 4MB,支持存储1080P及以下图片

    uint32_t graph_id;
    uint32_t src_engine_id;
    uint32_t src_port_id;
    uint32_t dst_engine_id;
    uint32_t dst_port_id;

    int width;
    int height;

    CodeTypeE codeType;

    std::shared_ptr<DvppEncodeRecvInterface> callback;
    std::shared_ptr<hiai::Graph> graph;

    std::queue<DvppEngineTensorT> dataQueue;
    std::mutex dataMtx;
    std::condition_variable consume;

    int framecount;
    std::shared_ptr<DvppEngineTensorT> framedata;
};
} // namespace hilens
#endif // CLOUD
#endif // LIBHILENS_DVPP_ENCODER_H