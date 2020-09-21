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

#ifndef LIBHILENS_FILE_AUDIO_H
#define LIBHILENS_FILE_AUDIO_H

#include <stdio.h>
#include "audio_capture.h"
extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavdevice/avdevice.h>
#include <libswresample/swresample.h>
}

namespace hilens {
class FileAudio : public AudioCapture {
public:
    FileAudio() {}
    virtual ~FileAudio();

    /* 读取一帧音频 */
    virtual int Read(AudioFrame &frames, int n = 1);
    /* 音频参数 */
    virtual int SetProperty(const struct AudioProperties &properties) {}
    virtual int GetProperty(struct AudioProperties &properties);
    /* 音频音量 */
    virtual int SetVolume(int volume) {}
    virtual int GetVolume();

    /* *
     * @brief 音频初始化
     */
    bool Init()
    {
        return false;
    }
    bool Init(const std::string filePath);

private:
    /* 环境初始化标志 */
    static bool envCreated;
    /* 音频参数 */
    struct AudioProperties audioProperties;
    /* 音频来源0:MIC 1:音频文件 */
    AUDIO_INPUT_TYPE_E audioType;
    /* 音频帧临时缓存 */
    std::shared_ptr<void> defaultData;
    /* MIC录入音量大小，范围[-87, +86] */
    int volume;
    /* 通道数目 */
    int chnCnt;

    int audioIndex;

    AVFormatContext *pFormatCtx;
    AVCodecContext *codecCtx;
    AVCodec *cod;
    AVPacket *packet;
    AVFrame *avframe;
    struct SwrContext *convert_ctx;
    int buffer_size;
    uint8_t *buffer;
};
} // namespace hilens
#endif // LIBHILENS_FILE_AUDIO_H