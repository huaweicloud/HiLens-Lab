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

#ifndef HILENS_INCLUDE_VIDEO_CAPTURE_H
#define HILENS_INCLUDE_VIDEO_CAPTURE_H

#include <memory>
#include <string>
#include "media_process.h"

namespace hilens {
/* *
 * @brief 视频采集器
 * 使用视频采集器来读取本地摄像头或IP摄像头的数据
 */
class VideoCapture {
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
     * 视频帧宽度必须为16的倍数，高为2的倍数，且宽高的最小值为128
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

    /* *
     * @brief 析构视频采集器
     */
    virtual ~VideoCapture() {}

    /* *
     * @brief 读取一帧视频
     * 如果摄像头(IPC)读取发生错误，此接口将会抛出一个std::runtime_error
     * @return YUV_NV21的视频数据，如果是IPC或本地摄像头，则返回的是YUV_NV21的数据，如果是UVC摄像头，则返回JPEG数据
     */
    virtual cv::Mat Read() = 0;

    /* *
     * @brief 返回视频原始帧宽度
     * @return 视频宽度
     */
    virtual int Width() = 0;

    /* *
     * @brief 返回视频原始帧高度
     * @return 视频高度
     */
    virtual int Height() = 0;

protected:
    VideoCapture() {}
    VideoCapture(const VideoCapture &);
};
} // namespace hilens
#endif // HILENS_INCLUDE_VIDEO_CAPTURE_H
