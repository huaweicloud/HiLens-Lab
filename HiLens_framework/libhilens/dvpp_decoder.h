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

#ifndef LIBHILENS_DVPP_DECODER_H
#define LIBHILENS_DVPP_DECODER_H

#ifdef CLOUD

#include <memory>
#include <queue>
#include <condition_variable>

#include "model.h"
#include "hiai_common.h"
#include "vdecfactory.h"
#include "BlockingQueue.h"

namespace hilens {
class DvppDecoder;

// ????
class DvppInferRecvInterface : public hiai::DataRecvInterface {
public:
    DvppInferRecvInterface();
    ~DvppInferRecvInterface() {}
    HIAI_StatusT RecvData(const std::shared_ptr<void> &message);
    void SetDepend(DvppDecoder *dependDvppImpl);

    std::shared_ptr<DvppEngineTensorT> output;
    DvppDecoder *dependDvppImpl;
};

class DvppDecoder : public VDecFactoryInterface {
public:
    DvppDecoder(const std::string &filepath);
    virtual ~DvppDecoder();

    bool Init(bool bfast);
    HiLensEC Decode(const unsigned char *data, unsigned int size);
    void AddData(const DvppEngineTensorT &data);
    virtual void RegisterCallback(std::function<void(cv::Mat &)> cb);
    virtual int DecodeFrameBuf(unsigned char *inData, unsigned int inSize);

private:
    int SetCallback();

private:
    uint32_t graph_id;
    uint32_t src_engine_id;
    uint32_t src_port_id;
    uint32_t dst_engine_id;
    uint32_t dst_port_id;

    std::shared_ptr<DvppInferRecvInterface> callback;
    std::shared_ptr<hiai::Graph> graph;
    CodeTypeE codeType;

    std::shared_ptr<DvppEngineTensorT> framedata;
    std::function<void(cv::Mat &)> decCb;
    BlockingQueue<cv::Mat> imgqueue;
    std::mutex sendmtx;
    bool bIsFast;
};
} // namespace hilens
#endif // CLOUD
#endif // LIBHILENS_DVPP_DECODER_H
