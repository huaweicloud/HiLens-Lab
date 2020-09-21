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

#ifndef LIBHILENS_LOCAL_AUDIO_H
#define LIBHILENS_LOCAL_AUDIO_H

#include <stdio.h>
#include "audio_capture.h"
extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavdevice/avdevice.h>
#include <libswresample/swresample.h>
}

namespace hilens {
class LocalAudio : public AudioCapture {
public:
    LocalAudio() {}
    virtual ~LocalAudio();

    /* 读取一帧音频 */
    virtual int Read(AudioFrame &frames, int n = 1);
    /* 音频参数设置 */
    virtual int SetProperty(const struct AudioProperties &properties);
    virtual int GetProperty(struct AudioProperties &properties);
    /* 音频录入音量设置 */
    virtual int SetVolume(int vol);
    virtual int GetVolume();

    /* *
     * @brief 音频初始化
     */
    bool Init();
    bool Init(const std::string filePath);

private:
    bool Init_Split(const std::string &filePath);
    int ReadFromFile(int n);
    int GetWavdata(int frameIndex);
    /* public */
    /* 音频来源0:MIC 1:音频文件 */
    AUDIO_INPUT_TYPE_E audioType;
    /* 音频帧临时缓存 */
    std::shared_ptr<void> defaultData;
    /* 一次读取最大音频帧数目 */
    int maxFrmNum;
    /* 一帧音频大小 */
    int frameSize;

    /* ffmpeg for file */
    int audioIndex;
    int bytesPersample;
    bool dftDataInit;
    AVFormatContext *pFormatCtx;
    AVCodecContext *codecCtx;
    AVCodec *cod;
    AVPacket *packet;
    AVFrame *avframe;
    struct SwrContext *convert_ctx;
    int buffer_size;
    unsigned char *buffer;
};
} // namespace hilens
#endif // LIBHILENS_LOCAL_AUDIO_H