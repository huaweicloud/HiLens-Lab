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

#include "rtmp_publisher.h"
#include <sys/time.h>
#include <unistd.h>
#include "sfw_log.h"

using namespace hilens;
using namespace std;
using namespace cv;

bool RTMPPublisher::Init(const string &url)
{
    this->url = url;
    connected = false;
    rtmp = NULL;
    encoderInited = false;
    reconnectThreshold = 10;
    reconnectIndex = 0;
    idx = 0;

    encoder = new (std::nothrow) VEncoder();
    if (nullptr == encoder) {
        ERROR("Failed to construct VEncoder");
        return false;
    }

    // 初始化时就开始连接
    if (0 != Connect()) {
        ERROR("Failed to connect rtmp server");
        return false;
    }

    return true;
}

RTMPPublisher::~RTMPPublisher()
{
    if (connected) {
        Disconnect();
    }
    if (encoder) {
        delete encoder;
    }
}

int RTMPPublisher::Connect()
{
    if (NULL != rtmp) {
        return 0;
    }

    if (url.empty()) {
        ERROR("Failed to get valid RTMP url!");
        return 1;
    }

    INFO("srs connecting %s", url.c_str());

    rtmp = srs_rtmp_create(url.c_str());
    if (NULL == rtmp) {
        ERROR("srs_rtmp create failed");
        return 1;
    }

    if (0 != srs_rtmp_handshake(rtmp)) {
        ERROR("srs_rtmp simple handshake failed.");
        return 1;
    }
    if (0 != srs_rtmp_connect_app(rtmp)) {
        ERROR("connect vhost/app failed.");
        return 1;
    }
    if (0 != srs_rtmp_publish_stream(rtmp)) {
        ERROR("publish stream failed.");
        return 1;
    }

    time_t t1;
    gettimeofday(&tv, nullptr);
    t1 = tv.tv_sec * 1000000 + tv.tv_usec;
    startTime = (int)(t1 / 1000);

    tpublish = thread(&RTMPPublisher::PubThread, this);

    connected = true;
    return 0;
}

int RTMPPublisher::Disconnect()
{
    connected = false;
    tpublish.join();
    srs_rtmp_destroy(rtmp);
    rtmp = NULL;
}

HiLensEC RTMPPublisher::Show(const Mat &frame)
{
    if (!encoderInited) {
        if (0 != encoder->Init(frame.cols, frame.rows * 2 / 3)) {
            ERROR("Failed to init encoder");
            return UNKNOWN_ERROR;
        }
        encoderInited = true;
    }

    mtx.lock();
    frames.push_back(frame);
    // 丢弃过多的缓存，以免网络拥堵时内存暴涨
    if (frames.size() > 10) {
        frames.pop_front();
    }
    mtx.unlock();
    return OK;
}

void RTMPPublisher::PubThread()
{
    unsigned char *framedata = nullptr; // h264帧数据块
    unsigned int framesize = 0;      // h264帧数据块大小

    while (connected) {
        // 等待新的输入
        Mat frame = GetInputFrame();

        // 编码，如果编码失败则忽略该帧
        framedata = nullptr;
        framesize = 0;
        if (0 != encoder->EncodeFrame(frame.data, frame.total() * frame.elemSize(), &framedata, &framesize)) {
            if (framedata) {
                free(framedata);
            }
            continue;
        }

        // 按RTMP协议发送H264帧
        SendH264Data((char *)framedata, framesize);

        free(framedata);
    }
}

Mat RTMPPublisher::GetInputFrame()
{
    while (true) {
        usleep(100);
        mtx.lock();
        bool empty = frames.empty();
        if (!empty) {
            Mat frame = frames.front();
            frames.pop_front();
            mtx.unlock();
            return frame;
        }
        mtx.unlock();
    }
}

int RTMPPublisher::GetTimestamp()
{
    time_t t1;
    gettimeofday(&tv, nullptr);
    t1 = tv.tv_sec * 1000000 + tv.tv_usec;
    int currentTime = (int)(t1 / 1000);
    return currentTime - startTime;
}

const char *RTMPPublisher::GetFrameType(char nut)
{
    switch (nut) {
        case 7:
            return "SPS";
        case 8:
            return "PPS";
        case 5:
            return "I";
        case 1:
            return "P";
        default:
            return "Unknown";
    }
}

int RTMPPublisher::SendH264Data(const char *framedata, int framesize)
{
    char *h264_raw = (char *)framedata;
    char *p = h264_raw;
    int h264_size = framesize;

    for (; p < h264_raw + h264_size;) {
        // @remark, read a frame from buffer.
        char *data = nullptr;
        int size = 0;
        int nb_start_code = 0;
        if (ReadH264Frame(h264_raw, h264_size, &p, &nb_start_code, &data, &size) != 0) {
            ERROR("read a frame from buffer failed.");
            break;
        }

        // 5bits, 7.3.1 NAL unit syntax,
        // H.264-AVC-ISO_IEC_14496-10.pdf, page 44.
        const char *frametype = GetFrameType((char)data[nb_start_code] & 0x1f);

        LOG_DEBUG("srs sending raw frames...[idx: %d; size: %d; type:%s]", idx++, size, frametype);
        // send out the h264 packet over RTMP
        int dts = GetTimestamp();
        int pts = dts;
        int ret = srs_h264_write_raw_frames(rtmp, data, size, dts, pts);
        if (ret != 0) {
            DealError(ret);
        } else {
            LOG_DEBUG("srs sent packet: type=%s, time=%d, size=%d, b[%d]=%#x(%s)",
                srs_human_flv_tag_type2string(SRS_RTMP_TYPE_VIDEO), dts, framesize, nb_start_code,
                (char)data[nb_start_code], frametype);
        }
    }

    return 0;
}

void RTMPPublisher::DealError(int ret)
{
    if (srs_h264_is_dvbsp_error(ret)) {
        LOG_DEBUG("srs ignore drop video error, code=%d", ret);
    } else if (srs_h264_is_duplicated_sps_error(ret)) {
        LOG_DEBUG("srs ignore duplicated sps, code=%d", ret);
    } else if (srs_h264_is_duplicated_pps_error(ret)) {
        LOG_DEBUG("srs ignore duplicated pps, code=%d", ret);
    } else {
        ERROR("srs send h264 raw data failed. ret=%d", ret);
        // 此处可能是网络故障了，尝试重连一下
        reconnectIndex++;
        if (reconnectIndex >= reconnectThreshold) {
            WARN("rtmp: maybe network is broken, trying to reconnect...");
            reconnectIndex = 0;
            while (0 != Reconnect()) {
                sleep(1);
            }
            INFO("rtmp: reconnected.");
        }
    }
}

int RTMPPublisher::ReadH264Frame(char *data, int size, char **pp, int *pnb_start_code, char **frame, int *frame_size)
{
    char *p = *pp;

    if (!srs_h264_startswith_annexb(p, size - (p - data), pnb_start_code)) {
        ERROR("h264 raw data invalid.1");
        return 1;
    }

    *frame = p;
    p += *pnb_start_code;

    for (; p < data + size; p++) {
        if (srs_h264_startswith_annexb(p, size - (p - data), NULL)) {
            break;
        }
    }

    *pp = p;
    *frame_size = p - *frame;
    if (*frame_size <= 0) {
        ERROR("h264 raw data invalid.2");
        return 1;
    }
    return 0;
}

int RTMPPublisher::Reconnect()
{
    srs_rtmp_destroy(rtmp);

    sleep(2);

    rtmp = srs_rtmp_create(url.c_str());
    if (0 != srs_rtmp_handshake(rtmp)) {
        ERROR("srs_rtmp simple handshake failed.");
        return 1;
    }
    if (0 != srs_rtmp_connect_app(rtmp)) {
        ERROR("connect vhost/app failed.");
        return 1;
    }
    if (0 != srs_rtmp_publish_stream(rtmp)) {
        ERROR("publish stream failed.");
        return 1;
    }

    time_t t1;
    gettimeofday(&tv, nullptr);
    t1 = tv.tv_sec * 1000000 + tv.tv_usec;
    startTime = (int)(t1 / 1000);
    return 0;
}