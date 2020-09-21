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

#include <securec.h>
#include <string>
#include "utils.h"
#include "sf_common.h"
#include "sfw_log.h"
#include "file_audio.h"

const int MAX_NUM_FRAME = 512;

using namespace hilens;
using namespace std;

/* 检测文件是否为aac编码，并提取相关参数 */
int AudioFileCheckFile(const std::string filePath, struct AudioProperties &property, int &chnCnt)
{
    int audio_stream_idx;
    AVStream *audio_stream = NULL;

    /* FFmpeg提取音频参数 */
    AVFormatContext *ic = avformat_alloc_context();
    if (avformat_open_input(&ic, filePath.c_str(), NULL, NULL) < 0) {
        ERROR("could not open source %s", filePath.c_str());
        goto ERR;
    }

    if (avformat_find_stream_info(ic, NULL) < 0) {
        ERROR("could not find stream information");
        goto ERR;
    }

    audio_stream_idx = av_find_best_stream(ic, AVMEDIA_TYPE_AUDIO, -1, -1, NULL, 0);
    if (audio_stream_idx >= 0) {
        audio_stream = ic->streams[audio_stream_idx];
    } else {
        ERROR("av_find_best_stream failed");
        goto ERR;
    }

    if (strcmp("aac", avcodec_get_name(audio_stream->codecpar->codec_id))) {
        ERROR("Only support decode aac encode audio file so far!\n");
        goto ERR;
    }

    property.u32PtNumPerFrm = audio_stream->codecpar->frame_size;
    property.enSamplerate = audio_stream->codecpar->sample_rate;
    if (audio_stream->codecpar->format >= AV_SAMPLE_FMT_U8 && audio_stream->codecpar->format <= AV_SAMPLE_FMT_S16) {
        property.enBitwidth = audio_stream->codecpar->format;
    } else {
        property.enBitwidth = 1; /* 0:8bit 1:16bit 2:24bit */
    }

    chnCnt = audio_stream->codecpar->channels;

    avformat_close_input(&ic);
    return audio_stream_idx;

ERR:
    avformat_close_input(&ic);
    return -1;
}

bool FileAudio::envCreated = false;

bool FileAudio::Init(const std::string filePath)
{
    if (!envCreated) {
        audioIndex = AudioFileCheckFile(filePath, audioProperties, chnCnt);
        if (audioIndex < 0) {
            return false;
        }

        // 分配一个avformat
        pFormatCtx = avformat_alloc_context();
        if (pFormatCtx == NULL) {
            ERROR("local audio avformat_alloc_context failed!");
            return false;
        }

        // 打开文件，解封装
        if (avformat_open_input(&pFormatCtx, filePath.c_str(), NULL, NULL) != 0) {
            ERROR("local audio avformat_open_input open file failed!");
            return false;
        }

        // 查找文件的相关流信息
        if (avformat_find_stream_info(pFormatCtx, NULL) < 0) {
            ERROR("local audio find_stream_info failed!");
            return false;
        }

        // 输出格式信息
        INFO("--------------- File Information ----------------\n");
        av_dump_format(pFormatCtx, 0, filePath.c_str(), 0);
        INFO("-------------------------------------------------\n");

        cod = avcodec_find_decoder(pFormatCtx->streams[audioIndex]->codecpar->codec_id);
        codecCtx = avcodec_alloc_context3(cod);

        if (cod == NULL) {
            ERROR("local audio find decoder failed!");
            return false;
        }

        if (avcodec_open2(codecCtx, cod, NULL) < 0) {
            ERROR("local audio open decoder failed!");
            return false;
        }

        packet = (AVPacket *)malloc(sizeof(AVPacket));
        av_init_packet(packet);

        // 设置转码后输出相关参数
        // 采样个数
        int out_nb_samples = audioProperties.u32PtNumPerFrm;
        // 采样格式,目前kit上只支持S16格式
        enum AVSampleFormat sample_fmt = AV_SAMPLE_FMT_S16;
        // 采样率
        int out_sample_rate = audioProperties.enSamplerate;
        // 通道数
        int out_channels = chnCnt;
        // 通道数对应的采样布局方式
        uint64_t out_channel_layout = AV_CH_LAYOUT_STEREO;
        if (out_channels == 1) {
            out_channel_layout = AV_CH_LAYOUT_MONO;
        }

        // 创建buffer
        buffer_size = av_samples_get_buffer_size(NULL, out_channels, out_nb_samples, sample_fmt, 1);

        // 最大支持512帧
        defaultData.reset(new unsigned char[buffer_size * MAX_NUM_FRAME]);

        // 注意要用av_malloc
        buffer = (uint8_t *)av_malloc(buffer_size);

        // 创建Frame，用于存储解码后的数据
        avframe = av_frame_alloc();
        int64_t in_channel_layout = av_get_default_channel_layout(codecCtx->channels);
        // 打开转码器
        convert_ctx = swr_alloc();
        // 设置转码参数
        convert_ctx = swr_alloc_set_opts(convert_ctx, out_channel_layout, sample_fmt, out_sample_rate,
            in_channel_layout, codecCtx->sample_fmt, codecCtx->sample_rate, 0, NULL);
        // 初始化转码器
        swr_init(convert_ctx);

        envCreated = true;
    } else {
        ERROR("Audio system has been inited for file!\n");
        return true;
    }

    return envCreated;
}

FileAudio::~FileAudio()
{
    if (pFormatCtx) {
        avformat_close_input(&pFormatCtx);
        avformat_free_context(pFormatCtx);
    }
    if (codecCtx) {
        avcodec_close(codecCtx);
        avcodec_free_context(&codecCtx);
    }
    if (convert_ctx) {
        swr_free(&convert_ctx);
    }
    envCreated = false;
    if (buffer) {
        free(buffer);
        buffer = nullptr;
    }
    if (packet) {
        av_packet_unref(packet);
    }
    if (avframe) {
        av_frame_free(&avframe);
    }
}

/* 读取一帧音频 */
int FileAudio::Read(AudioFrame &frames, int n)
{
    /* 没有调用init()的对象，不允许read */
    if (!envCreated) {
        throw std::runtime_error("AudioCapture init failed!");
    }

    // 每次读取一帧
    for (int i = 0; i < n; i++) {
        if (av_read_frame(pFormatCtx, packet) >= 0) {
            if (packet->stream_index == audioIndex) {
                if (avcodec_send_packet(codecCtx, packet) == 0) {
                    while (avcodec_receive_frame(codecCtx, avframe) == 0) {
                        // 转码
                        swr_convert(convert_ctx, &buffer, buffer_size, (const uint8_t **)avframe->data,
                            avframe->nb_samples);
                        int ret =
                            memcpy_s((uint8_t *)defaultData.get() + i * buffer_size, buffer_size, buffer, buffer_size);
                        if (ret != 0) {
                            ERROR("memcpy_s failed, ret == %d\r\n", ret);
                            av_packet_unref(packet);
                            return -1;
                        }
                    }
                }
            }
            av_packet_unref(packet);
        }
    }
    /* 拷贝到智能指针 */
    frames.data = defaultData;
    frames.size = n * buffer_size;

    return 0;
}

int FileAudio::GetProperty(struct AudioProperties &properties)
{
    int ret = memcpy_s(&properties, sizeof(struct AudioProperties), &audioProperties, sizeof(struct AudioProperties));
    if (ret != 0) {
        ERROR("memcpy_s failed, ret == %d\r\n", ret);
    }
    return 0;
}

int FileAudio::GetVolume()
{
    return volume;
}
