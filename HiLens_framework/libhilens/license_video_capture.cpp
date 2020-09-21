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

#include "license_video_capture.h"
#include "local_camera.h"
#include "ipc_capture.h"
#include "uvc_capture.h"
#include "auth.h"
#include "sfw_log.h"
#include "hilens_errorcode.h"
#include "mp4_reader.h"

using namespace std;
using namespace hilens;

mutex LicenseVideoCapture::usedMtx;

// 构造本地摄像头
shared_ptr<VideoCapture> LicenseVideoCapture::Create()
{
    lock_guard<mutex> autolock(usedMtx);

    LocalCamera *cam = new (std::nothrow) LocalCamera();
    if (cam != nullptr) {
        if (!cam->Init()) {
            delete cam;
            return nullptr;
        }
    }

    return std::shared_ptr<VideoCapture>(cam);
}

// 构造IPC
shared_ptr<VideoCapture> LicenseVideoCapture::Create(const string &name)
{
    // 使用宽高默认值(0,0)表示使用视频帧原始宽和高
    return Create(name, 0, 0);
}

shared_ptr<VideoCapture> LicenseVideoCapture::Create(const std::string &name, const unsigned int width,
    const unsigned int height)
{
    lock_guard<mutex> autolock(usedMtx);

    // 支持读取MP4文件
    if (name.size() > 4 && name.substr(name.size() - 4, 4) == ".mp4") {
        MP4Reader *cam = new (std::nothrow) MP4Reader(width, height);
        return Init(cam, name);
    } else {
        IPCCapture *cam = new (std::nothrow) IPCCapture(width, height);
        return Init(cam, name);
    }
}

shared_ptr<VideoCapture> LicenseVideoCapture::Create(int dev)
{
    lock_guard<mutex> autolock(usedMtx);

    UVCCapture *cam = new (std::nothrow) UVCCapture();
    if (cam != nullptr) {
        if (!cam->Init(dev)) {
            delete cam;
            return nullptr;
        }
    }

    return std::shared_ptr<VideoCapture>(cam);
}

LicenseVideoCapture::LicenseVideoCapture() {}

LicenseVideoCapture::~LicenseVideoCapture() {}
