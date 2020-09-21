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

#ifndef HILENS_SECURITY_VDECODER_H
#define HILENS_SECURITY_VDECODER_H

#include <HiLensMedia.h>
#include "vdecfactory.h"
#include <mutex>
#include "BlockingQueue.h"
#include <thread>
#include <atomic>
namespace hilens {
class VDecoder : public VDecFactoryInterface {
public:
    // MPP VPSS组件支持的最小宽高为128像素(高可能可以支持更小，但是意义不大，这里保持和宽的统一)
    static const int MIN_WIDTH = 128;
    static const int MIN_HEIGHT = 128;

    // MPP VPSS组件的约束：视频宽必须是16的倍数，高必须是2的倍数
    // MPP
    // VPSS组件图片缓存的宽是32字节对齐的。也就是说视频宽是16的倍数，但是不是32的倍数情况下，图片会padding16像素的黑边
    static const int MIN_ALIGN_WIDTH = 16;
    static const int MIN_ALGIN_HEIGHT = 2;
    static const int ALIGN_MEMORY_WIDTH = 32;

public:
    VDecoder(unsigned int width, unsigned int height, ENCODE_TYPE_E codec);
    VDecoder(unsigned int width, unsigned int height, unsigned int destWidth, unsigned int destHeight,
        ENCODE_TYPE_E codec);

    int Init(bool bfast);
    virtual ~VDecoder();

    // 输入数据，获得输出数据
    virtual void RegisterCallback(std::function<void(cv::Mat &)> cb);
    virtual int DecodeFrameBuf(unsigned char *inData, unsigned int inSize);

private:
    MediaHandle handle;
    ENCODE_TYPE_E codec;

    unsigned int width;
    unsigned int height;

    unsigned int destWidth;
    unsigned int destHeight;
    unsigned int alignWidth;
    std::function<void(cv::Mat &)> decCb;
    std::mutex initmtx;
    std::atomic_bool running;
    std::thread decthread;
    void decthreadFunc();
    BlockingQueue<int> decqueue;
    bool bIsFast;

    static std::atomic_int g_usedCount;
};
} // namespace hilens
#endif // HILENS_SECURITY_VDECODER_H
