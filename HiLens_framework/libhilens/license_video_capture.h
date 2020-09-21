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

#ifndef LIBHILENS_LICENSE_VIDEO_CAPTURE_H
#define LIBHILENS_LICENSE_VIDEO_CAPTURE_H

#include "video_capture.h"
#include <mutex>
#include <atomic>

namespace hilens {
class LicenseVideoCapture : public VideoCapture {
public:
    /* *
     * @brief 构造视频采集器（本地摄像头）
     * @return 视频采集器实例
     */
    static std::shared_ptr<VideoCapture> Create();
    /* *
     * @brief 构造视频采集器
     * @param name
     * 设备配置中的摄像头名（设备配置中的IPC）。优先从设备配置中的摄像头名称，也可以直接传入形如rtsp://xxx的取流地址
     * @return 视频采集器实例
     */
    static std::shared_ptr<VideoCapture> Create(const std::string &name);

    /* *
     * @brief 构造视频采集器
     * @param name 设备配置中的摄像头名（设备配置中的IPC）。
     * 优先从设备配置中的摄像头名称，也可以直接传入形如rtsp://xxx的取流地址，还支持读取本地mp4文件
     * @param width 设置读取到的视频帧图片宽度（要求为16的倍数，推荐为32的倍数，且最小为128）
     * @param height 设置读取到的视频帧图片高度（要求为2的倍数，且最小为128）
     * @return 视频采集器实例
     */
    static std::shared_ptr<VideoCapture> Create(const std::string &name, const unsigned int width,
        const unsigned int height);
    /* *
     * @brief 构造视频采集器
     * @param dev /dev中的UVC摄像头ID
     * @return 视频采集器实例
     */
    static std::shared_ptr<VideoCapture> Create(int dev);

protected:
    LicenseVideoCapture();
    virtual ~LicenseVideoCapture();

private:
    template <typename T1, typename T2> static std::shared_ptr<VideoCapture> Init(T1 instance, T2 name)
    {
        if (instance != nullptr) {
            if (!instance->Init(name)) {
                delete instance;
                return nullptr;
            }
        }

        return std::shared_ptr<VideoCapture>(instance);
    }

private:
    static std::mutex usedMtx;
};
} // namespace hilens
#endif // LIBHILENS_LICENSE_VIDEO_CAPTURE_H