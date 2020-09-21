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

#ifndef LIBHILENS_MODEL_IMPL_H
#define LIBHILENS_MODEL_IMPL_H

#include <memory>
#include "model.h"
#include "hiai_common.h"
#include "BlockingQueue.h"

namespace hilens {
// 回调函数
class InferRecvInterface : public hiai::DataRecvInterface {
public:
    InferRecvInterface(BlockingQueue<InferDataVec> *pqinfer, BlockingQueue<int> *pqsend)
    {
        flag = false;
        pinferque = pqinfer;
        psendque = pqsend;
    }
    ~InferRecvInterface(){}
    HIAI_StatusT RecvData(const std::shared_ptr<void> &message);
    bool flag; // 是否接收完成
    std::shared_ptr<InferEngineOutputT> output;
    BlockingQueue<InferDataVec> *pinferque;
    BlockingQueue<int> *psendque;
};

class ModelImpl : public Model {
public:
    ModelImpl(const std::string &filepath) : filepath(filepath) {}
    virtual ~ModelImpl();
    bool Init();
    virtual HiLensEC Infer(const InferDataVec &inputs, InferDataVec &outputs);

private:
    uint32_t graph_id;
    uint32_t src_engine_id;
    uint32_t src_port_id;
    uint32_t dst_engine_id;
    uint32_t dst_port_id;
    std::shared_ptr<InferRecvInterface> callback;
    std::shared_ptr<hiai::Graph> graph;
    std::string filepath;
    int SetCallback();
    BlockingQueue<InferDataVec> inferque;
    BlockingQueue<int> sendque;
    std::mutex infermtx;
};
} // namespace hilens
#endif // LIBHILENS_MODEL_IMPL_H