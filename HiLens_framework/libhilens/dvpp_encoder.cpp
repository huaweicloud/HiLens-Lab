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

#ifdef CLOUD
#include "dvpp_encoder.h"
#include <string>
#include <unistd.h>
#include "resource.h"
#include "sfw_log.h"
#include "hiaiengine/ai_memory.h"

using namespace std;

namespace hilens {
// 获取一个graph的拷贝
string GetGraphEncode(const string &graphName, int id)
{
    // 拼接路径
    string newId = to_string(id);
    string srcGraph = string("/home/hilens/skillframework/configs/") + graphName;
    string dstGraph = graphName + newId;

    // 复制文件
    string cmdcp = string("cp ") + srcGraph + " " + dstGraph;
    system(cmdcp.c_str());

    // 替换dst中的id
    const char *flag_graph_id = "replace_graph_id";
    string cmd = string("sed -i \"s/") + flag_graph_id + "/" + newId + "/g\" " + dstGraph;
    system(cmd.c_str());

    return dstGraph;
}

DvppEncodeRecvInterface::DvppEncodeRecvInterface() : flag(false), dependDvppImpl(nullptr) {}

void DvppEncodeRecvInterface::SetDepend(DvppEncoder *dependDvppImpl)
{
    this->dependDvppImpl = dependDvppImpl;
}

// 接收Device侧的回调函数
HIAI_StatusT DvppEncodeRecvInterface::RecvData(const std::shared_ptr<void> &message)
{
    output = std::static_pointer_cast<DvppEngineTensorT>(message);
    if (nullptr == output) {
        ERROR("Infer: fail to receive data");
        return HIAI_ERROR;
    }

    this->dependDvppImpl->AddData(*output.get());
    flag = true;
    return HIAI_OK;
}

DvppEncoder::~DvppEncoder()
{
    INFO("DvppEncoder destruction.");

    graph = hiai::Graph::GetInstance(graph_id);
    if (nullptr != graph) {
        hiai::Graph::DestroyGraph(graph_id);
        INFO("DvppEncoder: graph(%u) destroyed.", graph_id);
    }
}

bool DvppEncoder::Init(int width, int height)
{
    static int offid = 20;

    graph_id = getpid() * 10 + offid++;
    string graphPath = GetGraphEncode("encode.graph", graph_id);

    graph = hiai::Graph::GetInstance(graph_id);
    if (nullptr != graph) {
        hiai::Graph::DestroyGraph(graph_id);
        INFO("Init Model: graph(%u) destroyed.", graph_id);
    }

    INFO("graph_id(%d), graphPath(%s)", graph_id, graphPath.c_str());

    // 替换graph中的模型文件
    // 替换dst中的id
    HIAI_StatusT status = hiai::Graph::CreateGraph(graphPath);
    if (status != HIAI_OK) {
        ERROR("Model: Failed to create model graph(hiai status: 0x%x)", status);
        return false;
    }

    // 清理graph
    string cmdclean = string("rm ") + "encode.graph" + to_string(graph_id);
    WARN("Cleaning graph(%s)", graphPath.c_str());
    system(cmdclean.c_str());

    src_engine_id = 1000;
    src_port_id = 0;
    dst_engine_id = 1004;
    dst_port_id = 0;

    if (0 != SetCallback()) {
        ERROR("Model: Failed to set callback");
        return false;
    }

    offid++;

    framedata->data.reset(new u_int8_t[FRAME_BUFF_SIZE]);
    framedata->size = 0;
    framecount = 0;

    this->width = width;
    this->height = height;

    return true;
}

int DvppEncoder::SetCallback()
{
    // 加载指定graph_id的graph
    graph = hiai::Graph::GetInstance(graph_id);
    if (nullptr == graph) {
        ERROR("Failed to get the graph %u", graph_id);
        return 1;
    }

    hiai::EnginePortID target_port_config;
    target_port_config.graph_id = graph_id;
    target_port_config.engine_id = dst_engine_id;
    target_port_config.port_id = 0;

    // 创建并设置回调类对象InferRecvInterface，shared指针callback指向InferRecvInterface
    callback = shared_ptr<DvppEncodeRecvInterface>(new DvppEncodeRecvInterface);
    graph->SetDataRecvFunctor(target_port_config, callback);
    callback->SetDepend(this);

    return 0;
}

void DvppEncoder::AddData(const DvppEngineTensorT &data)
{
    unique_lock<mutex> lck(dataMtx);

    if (dataQueue.size() >= QUEUE_NUM) {
        WARN("need pop framedata, size(%lu)", dataQueue.size());
        dataQueue.pop();
    }

    dataQueue.push(data);
}

int DvppEncoder::GetBuffSize()
{
    unique_lock<mutex> lck(dataMtx);
    return dataQueue.size();
}

void DvppEncoder::ReadData(InferData &data)
{
    unique_lock<mutex> lck(dataMtx);

    if (!dataQueue.empty()) {
        DvppEngineTensorT tmpData = dataQueue.front();
        data.size = tmpData.size;
        data.data = tmpData.data;
        dataQueue.pop();
    } else {
        data.size = 0;
        data.data.reset();
    }
}

HiLensEC DvppEncoder::Encode(const InferData &inputs)
{
    return Encode(inputs.data.get(), inputs.size);
}

HiLensEC DvppEncoder::Encode(const unsigned char *data, const int size)
{
    // 构造impl的输入
    auto implInput = make_shared<EncodeEngineTensorT>();

    if (size > FRAME_BUFF_SIZE) {
        ERROR("memory overflow.");
        framedata->size = 0;
        return HIAI_SEND_DATA_FAILED;
    } else {
        memcpy_s(framedata->data.get(), size, data, size);
        framedata->size = size;
    }

    implInput->size = framedata->size;
    implInput->data = framedata->data;
    implInput->height = this->height;
    implInput->width = this->width;
    implInput->codeType = codeType;

    HIAI_StatusT hiai_ret = HIAI_OK;
    hiai::EnginePortID engine_id;
    engine_id.graph_id = graph_id;
    engine_id.engine_id = src_engine_id;
    engine_id.port_id = 0;

    // 发送数据
    do {
        hiai_ret = graph->SendData(engine_id, "EncodeEngineTensorT", static_pointer_cast<void>(implInput));
        if (HIAI_QUEUE_FULL == hiai_ret) {
            INFO("[dvpp_encode] queue full, sleep 0.5ms");
            usleep(500);
        }
    } while (hiai_ret == HIAI_QUEUE_FULL);

    // 检查发送结果
    if (HIAI_OK != hiai_ret) {
        ERROR("[dvpp_encode] SendData failed! error code: 0x%x", hiai_ret);
        return HIAI_SEND_DATA_FAILED;
    }

    return OK;
}
} // namespace hilens
#endif // #ifdef CLOUD