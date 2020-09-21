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
#include <signal.h>
#include "utils.h"
#include "sf_common.h"
#include "sfw_log.h"
#include "local_audio.h"
#include "HiLensAudioRpc.h"
extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
}

using HI_U8 = unsigned char;
using HI_S32 = int;
using HI_U64 = unsigned long long;
using HI_S64 = long long;

#define HI_NULL 0L
#define HI_SUCCESS 0
#define HI_FAILURE (-1)

#define SAMPLE_AUDIO_PTNUM_PER_FRAME 1024
#define SAMPLE_AUDIO_DFT_CHN_CNT 2
#define AUDIO_BIT_WIDTH_16 1 /* 16bit width */

/* 音频系统初始化 */
bool gAudioSystemInit = false;
HI_S32 gAdecHandle;
HI_S32 gAencHandle;

using namespace hilens;
using namespace std;

/* 检测文件是否为aac编码，并提取相关参数 */
bool AudioFileCheck(const std::string filePath, struct AudioProperties &property, int &chnCnt)
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
    return true;

ERR:
    avformat_close_input(&ic);
    return false;
}

bool LocalAudio::Init()
{
    audioType = AUDIO_FROM_MIC;
    maxFrmNum = MAX_FRAME_NUM_ONCE;
    frameSize = SAMPLE_AUDIO_PTNUM_PER_FRAME * (AUDIO_BIT_WIDTH_16 + 1) * SAMPLE_AUDIO_DFT_CHN_CNT;
    defaultData.reset(new unsigned char[frameSize * maxFrmNum]);

    if (AudioRpcInit() != HILENS_SUCCESS) {
        ERROR("AudioRpcInit failed!\n");
        return false;
    }

    return true;
}

bool LocalAudio::Init_Split(const std::string &filePath)
{
    /* open the input file to read from it */
    if (avformat_open_input(&pFormatCtx, filePath.c_str(), NULL, NULL) != 0) {
        ERROR("local audio avformat_open_input open file failed!");
        return false;
    }
    /* get information on the input file */
    if (avformat_find_stream_info(pFormatCtx, NULL) < 0) {
        ERROR("local audio find_stream_info failed!");
        return false;
    }

    /* make sure that there is only one stream in the input file */
    if (pFormatCtx->nb_streams != 1) {
        ERROR("local audio input file has more than one stream!");
        return false;
    }

    audioIndex = av_find_best_stream(pFormatCtx, AVMEDIA_TYPE_AUDIO, -1, -1, NULL, 0);
    if (audioIndex < 0) {
        ERROR("local audio av_find_best_stream failed!");
        return false;
    }

    /* find a decoder for the audio stream */
    cod = avcodec_find_decoder(pFormatCtx->streams[audioIndex]->codecpar->codec_id);
    if (cod == NULL) {
        ERROR("local audio find decoder failed!");
        return false;
    }

    /* allocate a new decoding context */
    codecCtx = avcodec_alloc_context3(cod);
    if (codecCtx == NULL) {
        ERROR("local audio avcodec_alloc_context3 failed!");
        return false;
    }

    /* initialize the stream parameters with demuxer information */
    if (avcodec_parameters_to_context(codecCtx, pFormatCtx->streams[audioIndex]->codecpar) < 0) {
        ERROR("local audio get codec context failed!");
        return false;
    }

    /* open the deocder for the audio stream to use it later */
    if (avcodec_open2(codecCtx, cod, NULL) < 0) {
        ERROR("local audio open decoder failed!");
        return false;
    }

    /* dump input information to stderr */
    av_dump_format(pFormatCtx, 0, filePath.c_str(), 0);

    /* 创建Frame，用于存储解码后的数据 */
    avframe = av_frame_alloc();
    if (avframe == NULL) {
        ERROR("local audio av_frame_alloc failed!");
        return false;
    }

    packet = av_packet_alloc();
    if (packet == NULL) {
        ERROR("local audio av_packet_alloc failed!");
        return false;
    }
    av_init_packet(packet);
    packet->data = NULL;
    packet->size = 0;
    return true;
}


bool LocalAudio::Init(const std::string filePath)
{
    /* 全局变量初始化 */
    audioType = AUDIO_FROM_FILE;
    dftDataInit = false;
    maxFrmNum = MAX_FRAME_NUM_ONCE;

    /* alloc avformat context */
    pFormatCtx = avformat_alloc_context();
    if (pFormatCtx == NULL) {
        ERROR("local audio avformat_alloc_context failed!");
        return false;
    }

    if (!Init_Split(filePath)) {
        return false;
    }

    /* 设置转码后输出相关参数 */
    if (!strcmp("aac", avcodec_get_name(codecCtx->codec_id)) || !strcmp("mp3", avcodec_get_name(codecCtx->codec_id))) {
        /* 采样个数 */
        int out_nb_samples = codecCtx->frame_size;
        /* 采样格式,目前kit上只支持S16格式 */
        enum AVSampleFormat sample_fmt = AV_SAMPLE_FMT_S16;
        /* 采样率 */
        int out_sample_rate = codecCtx->sample_rate;
        /* 通道数 */
        int out_channels = codecCtx->channels;
        /* 通道数对应的采样布局方式 */
        HI_U64 out_channel_layout = AV_CH_LAYOUT_STEREO;
        if (out_channels == 1) {
            out_channel_layout = AV_CH_LAYOUT_MONO;
        }

        /* 创建buffer  */
        buffer_size = av_samples_get_buffer_size(NULL, out_channels, out_nb_samples, sample_fmt, 1);
        if (buffer_size <= 0) {
            ERROR("local audio buffer size(%d) error!", buffer_size);
            return false;
        }
        /* 最大支持512帧 */
        defaultData.reset(new unsigned char[buffer_size * maxFrmNum]);
        if (!defaultData) {
            ERROR("local audio defaultData new failed!");
            return false;
        }
        frameSize = buffer_size;
        dftDataInit = true;

        /* 注意要用av_malloc */
        buffer = (HI_U8 *)av_malloc(buffer_size);
        if (!buffer) {
            ERROR("local audio av_malloc failed!");
            return false;
        }
        HI_S64 in_channel_layout = av_get_default_channel_layout(codecCtx->channels);
        // 打开转码器
        convert_ctx = swr_alloc();
        // 设置转码参数
        convert_ctx = swr_alloc_set_opts(convert_ctx, out_channel_layout, sample_fmt, out_sample_rate,
            in_channel_layout, codecCtx->sample_fmt, codecCtx->sample_rate, 0, NULL);
        // 初始化转码器
        swr_init(convert_ctx);
    }

    return true;
}

LocalAudio::~LocalAudio()
{
    if (audioType == AUDIO_FROM_MIC) {
        if (AudioRpcDeInit() != HILENS_SUCCESS) {
            ERROR("AudioRpcDeInit failed!\n");
        }
    } else if (audioType == AUDIO_FROM_FILE) {
        if (pFormatCtx) {
            avformat_close_input(&pFormatCtx);
        }
        if (codecCtx) {
            avcodec_free_context(&codecCtx);
        }
        if (convert_ctx) {
            swr_free(&convert_ctx);
        }
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
}

int LocalAudio::GetWavdata(int frameIndex)
{
    if (!dftDataInit) {
        bytesPersample = av_get_bytes_per_sample(codecCtx->sample_fmt);
        if (bytesPersample <= 0) {
            ERROR("local audio av_get_bytes_per_sample failed!(size:%d)", bytesPersample);
            return HI_FAILURE;
        }
        if (avframe->nb_samples <= 0 || codecCtx->channels <= 0 || codecCtx->channels > 2) {
            ERROR("local audio input file is not completed or supported!");
            return HI_FAILURE;
        }
        frameSize = bytesPersample * avframe->nb_samples * codecCtx->channels;
        defaultData.reset(new unsigned char[frameSize * maxFrmNum]);
        dftDataInit = true;
    }
    HI_S32 s32Ret =
        memcpy_s((uint8_t *)defaultData.get() + frameIndex * frameSize, frameSize, avframe->data[0], frameSize);
    if (s32Ret != 0) {
        ERROR("memcpy_s failed, ret == %d\r\n", s32Ret);
        return HI_FAILURE;
    }
}

int LocalAudio::ReadFromFile(int n)
{
    HI_S32 s32Ret = 0;
    /* 每次读取一帧 */
    for (int i = 0; i < n; i++) {
        if ((s32Ret = av_read_frame(pFormatCtx, packet)) >= 0) {
            if (packet->stream_index == audioIndex) {
                /* raw packet data input to a decoder */
                if ((s32Ret = avcodec_send_packet(codecCtx, packet)) < 0) {
                    ERROR("local audio avcodec_send_packet failed!(error:%d)", s32Ret);
                    return HI_FAILURE;
                }
                /* decoded output data from a decoder */
                if ((s32Ret = avcodec_receive_frame(codecCtx, avframe)) < 0) {
                    ERROR("local audio avcodec_receive_frame failed!(error:%d)", s32Ret);
                    return HI_FAILURE;
                }

                if (!strcmp("aac", avcodec_get_name(codecCtx->codec_id)) ||
                    !strcmp("mp3", avcodec_get_name(codecCtx->codec_id))) {
                    /* 转码 */
                    swr_convert(convert_ctx, &buffer, buffer_size, (const uint8_t **)avframe->data,
                        avframe->nb_samples);
                    s32Ret = memcpy_s((uint8_t *)defaultData.get() + i * buffer_size, buffer_size, buffer, buffer_size);
                    if (s32Ret != 0) {
                        ERROR("memcpy_s failed, ret == %d\r\n", s32Ret);
                        return HI_FAILURE;
                    }
                } else {
                    GetWavdata(i);
                }
            }
        } else {
            if (s32Ret == AVERROR_EOF) {
                ERROR("local audio read to the of the file!");
                return HI_FAILURE;
            }
            ERROR("local audio could not read frame(err:%d)!", s32Ret);
            return HI_FAILURE;
        }
    }
    return HI_SUCCESS;
}

/* 读取一帧音频 */
int LocalAudio::Read(AudioFrame &frames, int n)
{
    HI_S32 s32Ret = 0;
    if (audioType == AUDIO_FROM_MIC) {
        if (AudioRpcRead((HI_U8 *)defaultData.get(), frameSize, n) != HILENS_SUCCESS) {
            ERROR("AudioRpcRead failed!\n");
            return HI_FAILURE;
        }
    } else {
        int ret = ReadFromFile(n);
        if (ret != HI_SUCCESS) {
            return ret;
        }
    }
    /* 拷贝到智能指针 */
    frames.data = defaultData;
    frames.size = n * frameSize;

    return HI_SUCCESS;
}


/* RPC调用使用默认参数，不支持设置，直接删除python有编译问题，先打桩，后续统一删除 */
int LocalAudio::SetProperty(const struct AudioProperties &properties)
{
    return HI_SUCCESS;
}

int LocalAudio::GetProperty(struct AudioProperties &properties)
{
    return HI_SUCCESS;
}

int LocalAudio::SetVolume(int vol)
{
    return HI_SUCCESS;
}

int LocalAudio::GetVolume()
{
    return HI_SUCCESS;
}
