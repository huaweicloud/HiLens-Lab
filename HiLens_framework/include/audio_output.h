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

#ifndef HILENS_INCLUDE_AUDIO_OUTPUT_H
#define HILENS_INCLUDE_AUDIO_OUTPUT_H

#include <string>
#include <memory>
#include <json/json.h>
#include "media_process.h"
#include "audio_capture.h"

namespace hilens {
/* *
 * @brief 使用音频输出口播放音频文件
 * @param filePath 播放音频文件路径
 * @param vol 播放音量，范围[-121, 6]
 * @return HiLensEC 0为成功，其他为失败
 */
HiLensEC PlayAacFile(const std::string filePath, int vol);
/*
 * @brief 播音异常处理
 */
void PlayHandleSigno(int signo);

/* *
 * @brief 音频播放
 * 使用AuioOutput类来将声音输出到AO接口 */
class AudioOutput {
public:
    /* *
     * @brief 构造输出，输入来源是MIC
     */
    static std::shared_ptr<AudioOutput> Create(const struct AudioProperties &property);

    /* *
     * @brief 构造输出，输入来源是音频文件
     */
    static std::shared_ptr<AudioOutput> Create(const std::string filePath);

    /* *
     * @brief 析构音频播放器
     */
    virtual ~AudioOutput() {}

    /* *
     * @brief 播放音频来源MIC
     */
    virtual int Play() = 0;

    /* *
     * @brief设置音频播放采样率、位宽和单帧采样点数
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
    AudioOutput() {}
    AudioOutput(const AudioOutput &);
};
} // namespace hilens
#endif // HILENS_INCLUDE_AUDIO_OUTPUT_H