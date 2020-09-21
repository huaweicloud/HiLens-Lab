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

#ifndef HILENS_INCLUDE_AUDIO_CAPTURE_H
#define HILENS_INCLUDE_AUDIO_CAPTURE_H

#include <memory>
#include <string>
#include "media_process.h"

/* 一次读取最大音频帧数，供python接口 */
#define MAX_FRAME_NUM_ONCE (512)

typedef struct AudioFrame_s {
    std::shared_ptr<void> data;
    unsigned int size;
} AudioFrame;

struct AudioProperties {
    unsigned int enSamplerate;   /* sample rate */
    unsigned int enBitwidth;     /* bitwidth */
    unsigned int u32PtNumPerFrm; /* point num per frame (80/160/240/320/480/1024/2048)
                                    (ADPCM IMA should add 1 point, AMR only support 160) */
};

typedef enum audio_input_TYPE_E {
    AUDIO_FROM_MIC = 0,
    AUDIO_FROM_FILE = 1,
    AUDIO_FROM_BUTT
} AUDIO_INPUT_TYPE_E;

namespace hilens {
/* *
 * @brief 音频采集器
 * 使用音频采集器来读取MIC 的数据
 */
class AudioCapture {
public:
    /* *
     * @brief 构造音频采集器(MIC作为输入，必须指定参数)
     * @return 音频采集器实例
     */
    static std::shared_ptr<AudioCapture> Create();

    /* *
     * @brief 析构音频采集器(音频文件为输入，参数通过ffmpeg提取头文件获取)
     */
    static std::shared_ptr<AudioCapture> Create(const std::string filePath);

    /* *
     * @brief 析构音频采集器
     */
    virtual ~AudioCapture() {}

    /* *
     * @brief 读取音频帧
     * 如果读取发生错误，此接口将会抛出一个std::runtime_error
     * @return PCM格式的音频数据
     */
    virtual int Read(AudioFrame &frames, int = 1) = 0;

    /* *
     * @brief设置音频MIC采样参数，主要是采样率、位宽和单帧采样点数
     *
     * @return
     */
    virtual int SetProperty(const struct AudioProperties &properties) = 0;
    virtual int GetProperty(struct AudioProperties &properties) = 0;

    /* *
     * @brief设置音频音量
     *
     * @return
     */
    virtual int SetVolume(int volume) = 0;
    virtual int GetVolume() = 0;

protected:
    AudioCapture() {}
    AudioCapture(const AudioCapture &);
};
} // namespace hilens
#endif // HILENS_INCLUDE_AUDIO_CAPTURE_H
