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

#ifndef LIBHILENS_VDECFACTORY_H
#define LIBHILENS_VDECFACTORY_H

#include <memory>
#include <opencv2/opencv.hpp>
#include <functional>

namespace hilens {
class VDecFactoryInterface {
public:
    template <class F, class... Args> inline std::function<void(cv::Mat &)> BindCallback(F &&f, Args &&... args)
    {
        return std::bind(std::forward<F>(f), std::forward<Args>(args)..., std::placeholders::_1);
    }

    virtual void RegisterCallback(std::function<void(cv::Mat &)> cb) = 0;
    virtual int DecodeFrameBuf(unsigned char *inData, unsigned int inSize) = 0;
};

using IVDec = std::shared_ptr<VDecFactoryInterface>;

class VDecFactory {
public:
    typedef enum _DecType {
        DecH265 = 0,
        DecH264 = 1
    } DecType;

    static IVDec Create(unsigned int width, unsigned int height, DecType codec, bool bfast);
    static IVDec Create(unsigned int width, unsigned int height, unsigned int destWidth, unsigned int destHeight,
        DecType codec, bool bfast);

private:
    VDecFactory() = default;
    ~VDecFactory() = default;
    VDecFactory(const VDecFactory &) = delete;
    VDecFactory &operator = (const VDecFactory &) = delete;
};
} // namespace hilens
#endif // LIBHILENS_VDECFACTORY_H
