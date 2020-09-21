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

#ifndef LIBHILENS_LOCAL_CAMERA_H
#define LIBHILENS_LOCAL_CAMERA_H

#include "license_video_capture.h"
#include <mutex>

#define CAM_WIDTH 1280
#define CAM_HEIGHT 720

namespace hilens {
class LocalCamera : public LicenseVideoCapture {
public:
    LocalCamera() {}
    virtual ~LocalCamera();

    virtual cv::Mat Read();
    virtual int Width();
    virtual int Height();

    // 非线程安全函数，由LicenseVideoCapture的create()调用时加锁保证线程安全
    bool Init();

private:
    bool envCreated = false;
};
} // namespace hilens
#endif // LIBHILENS_LOCAL_CAMERA_H