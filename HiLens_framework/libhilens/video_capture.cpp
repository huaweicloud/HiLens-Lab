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

#include "video_capture.h"
#include "license_video_capture.h"

using namespace std;
using namespace hilens;

static int g_usedCount = 0;

// 构造本地摄像头
shared_ptr<VideoCapture> VideoCapture::Create()
{
    return LicenseVideoCapture::Create();
}

// 构造IPC
shared_ptr<VideoCapture> VideoCapture::Create(const string &name)
{
    return LicenseVideoCapture::Create(name);
}

shared_ptr<VideoCapture> VideoCapture::Create(const std::string &name, const unsigned int width,
    const unsigned int height)
{
    return LicenseVideoCapture::Create(name, width, height);
}

// 构造UVC摄像头
shared_ptr<VideoCapture> VideoCapture::Create(int dev)
{
    return LicenseVideoCapture::Create(dev);
}
