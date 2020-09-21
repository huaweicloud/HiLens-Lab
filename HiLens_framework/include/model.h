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

#ifndef HILENS_INCLUDE_MODEL_H
#define HILENS_INCLUDE_MODEL_H

#include <memory>
#include <string>
#include <vector>
#include "media_process.h"
#include "errors.h"

namespace hilens {
/* *
 * @brief 一组模型模型推理数据
 */
struct InferData {
    unsigned int size;                   // 输出大小
    std::shared_ptr<unsigned char> data; // 数据指针
    /* *
     * @brief 构造一个空的模型推理数据
     */
    InferData() : size(0), data(nullptr) {}

    /* *
     * @brief 从一个cv::Mat构造一个InferData
     * @param img 输入图片
     */
    InferData(const cv::Mat &img);

    /* *
     * @brief 从一组指针数据构造一个InferData
     * @param data 数据指针，此构造函数会拷贝这部分数据
     * @param size 数据大小（字节）
     */
    InferData(const unsigned char *data, unsigned int size);
};

/* *
 * @brief 模型推理输入输出
 */
typedef std::vector<InferData> InferDataVec;

/* *
 * @brief 模型管理器
 * 使用模型管理器加载模型并进行推理
 */
class Model {
public:
    /* *
     * @brief 构造模型管理器
     * @param filename 模型文件路径。假设模型放在./mymodels/test.om，则filename为 "./mymodels/test.om"
     * @return 成功则模型管理器实例的指针，失败则返回nullptr
     */
    static std::shared_ptr<Model> Create(const std::string &filename);

    /* *
     * @brief 进行推理
     * 将数据输入模型进行推理，推理结束后将推理结果返回。
     * @param inputs 输入数据
     * @param outputs 输入数据
     * @return int 0推理成功，其他为失败。
     */
    virtual HiLensEC Infer(const InferDataVec &inputs, InferDataVec &outputs) = 0;

    /* *
     * @brief 销毁模型
     * Model析构时会销毁掉hiai::Graph等资源
     */
    virtual ~Model() {}

protected:
    Model() {}
    Model(const Model &);
};
} // namespace hilens
#endif // HILENS_INCLUDE_MODEL_H
