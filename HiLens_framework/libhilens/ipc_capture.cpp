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

#include "ipc_capture.h"
#include <stdexcept>
#include <unistd.h>
#include <securec.h>
#include <algorithm>
#include "sfw_log.h"
#include "sf_common.h"
#include "conf.h"
#include "MediaComm.h"

using namespace hilens;
using namespace std;
using namespace cv;

#define QUEUE_NUM 2 // ipc目前需要是2

IPCCapture::IPCCapture() : IPCCapture(0, 0) {}

IPCCapture::IPCCapture(const unsigned int destWidth, const unsigned int destHeight)
    : width(0),
      height(0),
      frameSize(0),
      updated(false),
      puller(nullptr),
      vDecoder(nullptr),
      frameData(nullptr),
      destHeight(destHeight),
      destWidth(destWidth)
{}

bool IPCCapture::InitDecoder()
{
    if (puller == nullptr) {
        return false;
    }

    puller->GetFrameSize(this->width, this->height);

    // 默认使用原视频帧宽高
    if (destWidth == 0 && destHeight == 0) {
        destHeight = this->height;
        destWidth = this->width;
    }

    INFO("origin width(%d), height(%d), dest width(%d), height(%d)", width, height, destWidth, destHeight);

    VDecFactory::DecType codec = VDecFactory::DecH265;
    if (puller->GetCodecType() == RTSPPuller::H264) {
        codec = VDecFactory::DecH264;
    }

    vDecoder = VDecFactory::Create(width, height, destWidth, destHeight, codec, false);
    if (vDecoder == nullptr) {
        ERROR("Failed to construct decode");
        return false;
    }
    vDecoder->RegisterCallback(vDecoder->BindCallback(&IPCCapture::DecCallback, this));

    this->frameSize = destHeight * destWidth * 3 / 2;
    return true;
}

void IPCCapture::DecCallback(cv::Mat &outimg)
{
    if (outimg.data) {
        imgqueue.ForcePush(outimg); /* 不阻塞，可能会超过上限+1，但是保证解码流畅 */
    } else {
        if (!imgqueue.Full()) {
            outimg = cv::Mat(destHeight * 3 / 2, destWidth, CV_8UC1);
        }
    }
}

bool IPCCapture::Init(const string &name)
{
    Hstring url;

    if (DeviceConfig::HasCamera(name)) {
        // name 对应设备配置中的摄像头
        url = DeviceConfig::GetCameraURL(name);
    } else if (name.find_first_of("rtsp://") != string::npos) {
        // name 直接为取流地址
        INFO("Open RTSP stream by URL directly.");
        url = name.c_str();
    } else {
        ERROR("Open VideoCapture failed(invalid name). Please check your IPC configs");
        return false;
    }

    // 从config中查找IPC
    if (url.empty()) {
        ERROR("Open video failed, empty url");
        return false;
    }

    puller.reset(new (std::nothrow) RTSPPuller(url));
    if (puller == nullptr) {
        ERROR("Failed to construct RTSPPuller");
        return false;
    }

    if (puller->Init() != 0) {
        ERROR("puller init failed!");
        return false;
    }

    if (!InitDecoder()) {
        return false;
    }
    imgqueue.SetCapacity(QUEUE_NUM);
    imgqueue.Start();

    // 启动拉流，并把拉到的数据解码
    pullStreamThread = thread(&RTSPPuller::Start, puller.get(), [&](uint8_t *data, int size) {
        if (!data) {
            imgqueue.ShutDown();
            return HILENS_SUCCESS;
        }
        // 解码
        if (vDecoder->DecodeFrameBuf(data, size) == 0) {
            return HILENS_SUCCESS;
        }

        WARN("Failed to decode video stream");
        return HILENS_FAILURE;
    });

    // 等待拉流线程启动ok
    while (!puller->IsStart()) {
        usleep(100000);
    }

    return true;
}

IPCCapture::~IPCCapture()
{
    INFO("IPCCapture destruction.");
    imgqueue.ShutDown();
    if (puller && pullStreamThread.joinable()) {
        puller->Stop();
        pullStreamThread.join();
    }
}

int IPCCapture::Width()
{
    return width;
}

int IPCCapture::Height()
{
    return height;
}

Mat IPCCapture::Read()
{
    Mat retMat;
    retMat.data = nullptr;
    imgqueue.Pop(retMat);

    if (!puller->IsRunning() || !retMat.data) {
        throw runtime_error("RTSP Puller stopped!");
    }

    return retMat;
}