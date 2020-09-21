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

#include "rtsp_puller.h"
#include <securec.h>
extern "C" {
#include <libavcodec/avcodec.h>
}
#include "sfw_log.h"

using namespace hilens;
using namespace std;

void RTSPPuller::InitFFMPEG()
{
    static bool inited = false;
    if (inited) {
        return;
    }

    avformat_network_init();
    inited = true;
}

RTSPPuller::RTSPPuller(const Hstring &url)
    : url(url),
      startFlag(false),
      running(false),
      pFormatCtx(nullptr),
      videoIndex(-1),
      type(H264),
      captureCallback(nullptr)
{}

RTSPPuller::~RTSPPuller()
{
    if (pFormatCtx) {
        avformat_close_input(&pFormatCtx);
    }
}

int RTSPPuller::Init()
{
    InitFFMPEG();

    pFormatCtx = avformat_alloc_context();
    AVDictionary *options = NULL;
    av_dict_set(&options, "rtsp_transport", "tcp", 0);
    av_dict_set(&options, "stimeout", "3000000", 0);

    int ret = avformat_open_input(&pFormatCtx, url.c_str(), 0, &options);
    if (ret < 0) {
        char buf[256];
        av_strerror(ret, buf, sizeof(buf));
        ERROR("Couldn't open input stream, %s", buf);
        av_dict_free(&options);
        return 1;
    }
    if (avformat_find_stream_info(pFormatCtx, NULL) < 0) {
        ERROR("Couldn't find stream information");
        return 1;
    }

    for (int i = 0; i < pFormatCtx->nb_streams; ++i) {
        if (pFormatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            videoIndex = i;
            break;
        }
    }
    if (videoIndex == -1) {
        ERROR("Failed to find video index\n");
        return 1;
    }

    INFO("The avcodec name is %s", avcodec_get_name(pFormatCtx->streams[videoIndex]->codecpar->codec_id));
    switch (pFormatCtx->streams[videoIndex]->codecpar->codec_id) {
        case AV_CODEC_ID_H264:
            this->type = H264;
            break;
        case AV_CODEC_ID_HEVC:
            this->type = HEVC;
            break;
        default:
            ERROR("Video stream codec not supported!");
            return 1;
    }

    return 0;
}

int RTSPPuller::Start(const RTSPCallback &callback)
{
    INFO("start pull stream thread, thread id = %llu", this_thread::get_id());
    running = true;
    startFlag = true; // 拉流线程启动OK标识，必须放在running标识后面
    captureCallback = callback;

    AVPacket pkt;
    av_init_packet(&pkt);

    int index = 0;
    int readFailedCount = 0;
    int callbackFailedCount = 0;

    while (running.load()) {
        int ret = av_read_frame(pFormatCtx, &pkt);
        if (ret < 0) {
            WARN("Read frame failed!\n");
            readFailedCount++;
            if (readFailedCount > 5) {
                ERROR("Too many reading frame failures. Stopping pulling stream!");
                Stop(); // 失败的帧过多，停止拉流
            }
        } else if (pkt.stream_index == videoIndex) {
            readFailedCount = 0;

            // 获得了frame数据，调用回调函数
            if (0 != callback(pkt.data, pkt.size)) {
                callbackFailedCount++;
                if (callbackFailedCount > 150) {
                    ERROR("Too many decoding callback failures. Stopping pulling stream!");
                    Stop(); // 失败的帧过多，停止拉流
                }
            } else {
                callbackFailedCount = 0;
            }
        } else {
            // 其他stream_index先忽略
        }
        av_packet_unref(&pkt);
    }

    INFO("quit pull stream thread");
}

void RTSPPuller::Stop()
{
    running = false;
    if (captureCallback)
        captureCallback(nullptr, 0);
}

int RTSPPuller::GetFrameSize(int &width, int &height)
{
    if (pFormatCtx == NULL || videoIndex == -1) {
        return -1;
    }
    height = pFormatCtx->streams[videoIndex]->codecpar->height;
    width = pFormatCtx->streams[videoIndex]->codecpar->width;
    return 0;
}

bool RTSPPuller::IsRunning()
{
    return running.load();
}

bool RTSPPuller::IsStart()
{
    return startFlag.load();
}

RTSPPuller::CodecType RTSPPuller::GetCodecType()
{
    return type;
}