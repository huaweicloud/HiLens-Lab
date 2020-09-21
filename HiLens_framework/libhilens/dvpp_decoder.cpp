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

#include "dvpp_decoder.h"

#ifdef CLOUD
#include <string>
#include <unistd.h>
#include "resource.h"
#include "sfw_log.h"
#include <hiaiengine/ai_memory.h>

using namespace std;
#define QUEUE_NUM 2
#define MAX_BUF_SIZE (1024 * 1024)

namespace hilens {
// 获取一个graph的拷贝
string GetGraphDecode(const string &graphName, int id)
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

DvppInferRecvInterface::DvppInferRecvInterface() : dependDvppImpl(nullptr) {}

void DvppInferRecvInterface::SetDepend(DvppDecoder *dependDvppImpl)
{
    this->dependDvppImpl = dependDvppImpl;
}

// 接收Device侧的回调函数
HIAI_StatusT DvppInferRecvInterface::RecvData(const std::shared_ptr<void> &message)
{
    output = std::static_pointer_cast<DvppEngineTensorT>(message);
    if (nullptr == output) {
        ERROR("Infer: fail to receive data");
        return HIAI_ERROR;
    }

    this->dependDvppImpl->AddData(*output.get());
    return HIAI_OK;
}

DvppDecoder::DvppDecoder(const std::string &filepath) : framedata(new DvppEngineTensorT)
{
    if (filepath == "h264") {
        codeType = CODE_TYPE_H264;
    } else {
        codeType = CODE_TYPE_H265;
    }
}

DvppDecoder::~DvppDecoder()
{
    INFO("DvppDecoder destruction.");

    graph = hiai::Graph::GetInstance(graph_id);
    if (nullptr != graph) {
        hiai::Graph::DestroyGraph(graph_id);
        INFO("DvppDecoder: graph(%u) destroyed.", graph_id);
    }
}

bool DvppDecoder::Init(bool bfast)
{
    bIsFast = bfast;
    static int offid = 0;

    graph_id = getpid() * 10 + offid++;
    string graphPath = GetGraphDecode("decode.graph", graph_id);

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
    string cmdclean = string("rm ") + "decode.graph" + to_string(graph_id);
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

    char *buffer = nullptr;
    HIAI_StatusT getRet =
        hiai::HIAIMemory::HIAI_DMalloc(MAX_BUF_SIZE, (void *&)buffer, 500, hiai::HIAI_MEMORY_ATTR_MANUAL_FREE);
    if (HIAI_OK != getRet || nullptr == buffer) {
        framedata->data.reset(new u_int8_t[MAX_BUF_SIZE]);
    } else {
        framedata->data.reset((u_int8_t *)buffer, DeleteDFree);
    }
    framedata->size = MAX_BUF_SIZE;
    imgqueue.SetCapacity(QUEUE_NUM);
    imgqueue.Start();

    return true;
}

int DvppDecoder::SetCallback()
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
    callback = shared_ptr<DvppInferRecvInterface>(new DvppInferRecvInterface);
    graph->SetDataRecvFunctor(target_port_config, callback);
    callback->SetDepend(this);

    return 0;
}

void DvppDecoder::AddData(const DvppEngineTensorT &data)
{
    cv::Mat outimg;
    if (data.data) {
        if (outimg.data) {
            memcpy_s(outimg.data, data.size, data.data.get(), data.size);
            decCb(outimg);
        }
    }
}

HiLensEC DvppDecoder::Decode(const unsigned char *data, unsigned int size)
{
    // 构造impl的输入,我猜这里是不是dma传输需要字节对齐，所以必须himalloc
    auto implInput = make_shared<DvppEngineTensorT>();

    if (size > framedata->size) {
        ERROR("memory overflow.");
        return HIAI_SEND_DATA_FAILED;
    } else {
        memcpy_s(framedata->data.get(), size, data, size); // 这个拷贝必须有的
    }

    implInput->size = size;
    implInput->data = framedata->data;
    implInput->codeType = codeType;

    HIAI_StatusT hiai_ret = HIAI_OK;
    hiai::EnginePortID engine_id;
    engine_id.graph_id = graph_id;
    engine_id.engine_id = src_engine_id;
    engine_id.port_id = 0;

    // 发送数据
    do {
        hiai_ret = graph->SendData(engine_id, "DvppEngineTensorT", static_pointer_cast<void>(implInput));
        if (HIAI_QUEUE_FULL == hiai_ret) {
            INFO("[Mind_face] queue full, sleep 0.5ms");
            usleep(500);
        }
    } while (hiai_ret == HIAI_QUEUE_FULL);

    // 检查发送结果
    if (HIAI_OK != hiai_ret) {
        ERROR("[dvpp_decode] SendData failed! error code: 0x%x", hiai_ret);
        return HIAI_SEND_DATA_FAILED;
    }

    return OK;
}

void DvppDecoder::RegisterCallback(std::function<void(cv::Mat &)> cb)
{
    decCb = cb;
}

int DvppDecoder::DecodeFrameBuf(unsigned char *inData, unsigned int inSize)
{
    usleep(10 * 1000); 
    return Decode(inData, inSize);
}
} // namespace hilens
#endif // #ifdef CLOUD