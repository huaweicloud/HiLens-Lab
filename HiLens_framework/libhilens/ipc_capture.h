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

#ifndef LIBHILENS_IPC_CAPTURE_H
#define LIBHILENS_IPC_CAPTURE_H

#include <memory>
#include <list>
#include <mutex>
#include <string>
#include <thread>
#include "license_video_capture.h"
#include "rtsp_puller.h"
#include "vdecfactory.h"
#include "BlockingQueue.h"

namespace hilens {
class IPCCapture : public LicenseVideoCapture {
public:
    IPCCapture();
    IPCCapture(const unsigned int destWidth, const unsigned int destHeight);
    virtual ~IPCCapture();

    virtual bool Init(const std::string &name);

    // 返回的是YVU_NV12的数据
    virtual cv::Mat Read();
    virtual int Width();
    virtual int Height();

private:
    bool InitDecoder();
    void DecCallback(cv::Mat &outimg);

private:
    std::unique_ptr<RTSPPuller> puller;
    IVDec vDecoder;
    std::unique_ptr<char> frameData;

    size_t frameSize;
    bool updated;

    int width;
    int height;
    int destWidth;
    int destHeight;

    std::mutex mtx;
    std::thread pullStreamThread;
    BlockingQueue<cv::Mat> imgqueue;
};
} // namespace hilens
#endif // LIBHILENS_IPC_CAPTURE_H
