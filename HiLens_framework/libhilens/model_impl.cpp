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

#include "model_impl.h"
#include <string>
#include <unistd.h>
#include "resource.h"
#include "sfw_log.h"

#define MAX_SEND_QUEUE 2

using namespace std;

namespace hilens {
// 获取一个graph的拷贝
string GetGraphCopy(const string &graphName, int id)
{
    // 拼接路径
    string newId = to_string(id);
    string srcGraph = string("/home/hilens/skillframework/configs/") + graphName;
    string dstGraph = GetWorkspacePath() + graphName + newId;

    // 复制文件
    string cmdcp = string("cp ") + srcGraph + " " + dstGraph;
    system(cmdcp.c_str());

    // 替换dst中的id
    const char *flag_graph_id = "replace_graph_id";
    string cmd = string("sed -i \"s/") + flag_graph_id + "/" + newId + "/g\" " + dstGraph;
    system(cmd.c_str());

    return dstGraph;
}

// 接收Device侧的回调函数
HIAI_StatusT InferRecvInterface::RecvData(const std::shared_ptr<void> &message)
{
    output = std::static_pointer_cast<InferEngineOutputT>(message);
    if (nullptr == output) {
        ERROR("Infer: fail to receive data");
        // 有些情况下，是会跑到这里的
        return HIAI_ERROR;
    }

    InferDataVec retdatav;
    retdatav.clear();
    for (auto tensor : output->vec) {
        InferData data;
        data.data = tensor.data;
        data.size = tensor.size;
        retdatav.push_back(data);
    }
    if (!output->errorMsg.empty()) {
        ERROR("Ascend 310: %s", output->errorMsg.c_str());
    }
    pinferque->ForcePush(retdatav);
    psendque->TryPopNull();

    return HIAI_OK;
}

ModelImpl::~ModelImpl()
{
    inferque.ShutDown();
    sendque.ShutDown();
    graph = hiai::Graph::GetInstance(graph_id);
    if (nullptr != graph) {
        hiai::Graph::DestroyGraph(graph_id);
        INFO("Model: graph(%u) destroyed.", graph_id);
    }
}

bool ModelImpl::Init()
{
    // 这个数是多少无所谓的只要不和其他graph的冲突了就行
    static int idoffset = 65;

    // gen id
    int gid;
    infermtx.lock();
    gid = getpid() * 100 + idoffset++;
    infermtx.unlock();
    string graphPath = GetGraphCopy("infer.graph", gid);
    // 替换graph中的模型文件
    // 替换dst中的id
    const char *flag_graph_model = "replace_graph_model";
    string cmd = string("sed -i 's#") + flag_graph_model + "#" + filepath + "#g' " + graphPath;
    system(cmd.c_str());

    HIAI_StatusT status = hiai::Graph::CreateGraph(graphPath);
    if (status != HIAI_OK) {
        ERROR("Model: Failed to create model graph(hiai status: 0x%x)", status);
        return false;
    }
    // 清理垃圾grap
    string cmdclean = string("rm ") + GetWorkspacePath() + "infer.graph*";
    system(cmdclean.c_str());

    graph_id = gid;
    src_engine_id = 1001;
    src_port_id = 0;
    dst_engine_id = 1002;
    dst_port_id = 0;

    if (0 != SetCallback()) {
        ERROR("Model: Failed to set callback");
        return false;
    }

    inferque.SetCapacity(0); // inferque的push不需要等待，所以无需配置最大值
    inferque.Start();
    sendque.SetCapacity(MAX_SEND_QUEUE);
    sendque.Start();

    return true;
}

int ModelImpl::SetCallback()
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
    target_port_config.port_id = dst_port_id;

    // 创建并设置回调类对象InferRecvInterface，shared指针callback指向InferRecvInterface
    callback = shared_ptr<InferRecvInterface>(new InferRecvInterface(&inferque, &sendque));
    graph->SetDataRecvFunctor(target_port_config, callback);

    return 0;
}

HiLensEC ModelImpl::Infer(const InferDataVec &inputs, InferDataVec &outputs)
{
    /* 这里主要是卡住， 不要发送过多的数据，DDK887缓存溢出直接会d310崩溃 */
    if (!sendque.Push(0)) {
        return HIAI_INFER_ERROR;
    }

    // 构造impl的输入
    auto implInput = make_shared<InferEngineInputT>();
    auto implOutput = make_shared<InferEngineOutputT>();

    for (auto data : inputs) {
        InferEngineTensorT t;
        t.size = data.size;
        t.data = data.data;
        implInput->vec.push_back(t);
    }

    HIAI_StatusT hiai_ret = HIAI_OK;
    hiai::EnginePortID engine_id;
    engine_id.graph_id = graph_id;
    engine_id.engine_id = src_engine_id;
    engine_id.port_id = src_port_id;

    // 发送数据
    do {
        hiai_ret = graph->SendData(engine_id, "InferEngineInputT", static_pointer_cast<void>(implInput));
        if (HIAI_QUEUE_FULL == hiai_ret) {
            INFO("[Mind_face] queue full, sleep 0.5ms");
            usleep(500);
        }
    } while (hiai_ret == HIAI_QUEUE_FULL);

    // 检查发送结果
    if (HIAI_OK != hiai_ret) {
        ERROR("[Mind_face] SendData failed! error code: 0x%x", hiai_ret);
        return HIAI_SEND_DATA_FAILED;
    }

    inferque.Pop(outputs);
    return outputs.size() > 0 ? OK : HIAI_INFER_ERROR;
}
} // namespace hilens
