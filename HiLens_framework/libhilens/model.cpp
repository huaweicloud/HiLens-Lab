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

#include "model.h"
#include <securec.h>
#include "model_impl.h"
#include "auth.h"
#include "sfw_log.h"
#include "hiaiengine/ai_memory.h"

using namespace std;
using namespace hilens;
using namespace cv;

InferData::InferData(const Mat &img)
{
    size = img.total() * img.elemSize();
    unsigned char *buff = new unsigned char[size];
    if (!buff) {
        ERROR("InferData new memory fail!");
    }
    memcpy_s(buff, size, img.data, size);
    data.reset(buff);
}

InferData::InferData(const unsigned char *data, unsigned int size)
{
    unsigned char *buff = nullptr;
    this->size = size;

    buff = new unsigned char[size];
    if (!buff) {
        ERROR("InferData new memory fail!");
    }
    this->data.reset(buff);

    if (data) {
        memcpy_s(buff, size, data, size);
    }
}

shared_ptr<Model> Model::Create(const string &filename)
{
    ModelImpl *model = new (std::nothrow) ModelImpl(filename);
    if (model) {
        if (!model->Init()) {
            delete model;
            return nullptr;
        }
    }
    return shared_ptr<Model>(model);
}
