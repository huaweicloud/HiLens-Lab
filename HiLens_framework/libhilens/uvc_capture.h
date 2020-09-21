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

#ifndef LIBHILENS_UVC_CAPTURE_H
#define LIBHILENS_UVC_CAPTURE_H

#include <mutex>
#include "license_video_capture.h"

/* 存放UVC摄像头一帧数据的缓存结构 */
typedef struct UvcFrameBuffer {
    void *start;
    size_t length;
} UvcFrameBuf;

namespace hilens {
class UVCCapture : public LicenseVideoCapture {
public:
    UVCCapture() {}
    virtual ~UVCCapture();
    bool Init(int dev);
    /* 返回的是MJPEG格式的数据 */
    virtual cv::Mat Read();
    virtual int Width();
    virtual int Height();

private:
    int devId;
    int fd;
    UvcFrameBuf *frameBuf;
    void *uvcBuffer;
    int width;
    int height;
    std::mutex mtx;
    /* 双路放权 */
    unsigned int readFrameNum;
    bool Check(int dev);
    bool SetParam();
    bool MallocBuffer();
    bool SetBuffer();
};
} /* namespace hilens */
#endif // LIBHILENS_UVC_CAPTURE_H