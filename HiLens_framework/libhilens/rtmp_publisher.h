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

#ifndef LIBHILENS_RTMP_PUBLISHER_H
#define LIBHILENS_RTMP_PUBLISHER_H

#include <list>
#include <mutex>
#include <string>
#include <sys/time.h>
#include <thread>
#include <srs_librtmp.h>
#include "media_process.h"
#include "vencoder.h"
#include "output.h"
#include "dvpp_encoder.h"

namespace hilens {
class RTMPPublisher : public Display {
public:
    RTMPPublisher() {}
    bool Init(const std::string &url);
    virtual ~RTMPPublisher();
    int Connect();
    int Disconnect();
    virtual HiLensEC Show(const cv::Mat &frame);

private:
    // 推流线程及相关函数
    void PubThread();
    // 获取输入的图片，如果没有输入则阻塞线程
    cv::Mat GetInputFrame();
    // 获取时间戳
    int GetTimestamp();
    const char *GetFrameType(char nut);
    // 发送h264数据
    int SendH264Data(const char *framedata, int framesize);
    int ReadH264Frame(char *data, int size, char **pp, int *pnb_start_code, char **frame, int *frame_size);
    // 处理发送错误的情况
    void DealError(int ret);
    int Reconnect();

private:
    bool connected;
    std::thread tpublish;
    srs_rtmp_t rtmp;
    bool encoderInited;
    VEncoder *encoder = nullptr;

    std::string url;
    int startTime;
    struct timeval tv;
    std::mutex mtx;
    std::list<cv::Mat> frames;

    // 判定需要重连的条件
    int reconnectThreshold;
    int reconnectIndex;
    int idx; // 发送的帧计数
};
} // namespace hilens
#endif // LIBHILENS_RTMP_PUBLISHER_H