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

#ifndef LIBHILESN_AUDIO_PLAY_H
#define LIBHILESN_AUDIO_PLAY_H

#include <stdio.h>
#include "audio_output.h"

namespace hilens {
/* *
 * @brief 音频播放
 * 使用AuioOutput类来将声音输出到AO接口 */
class AudioPlay : public AudioOutput {
public:
    AudioPlay() {}
    virtual ~AudioPlay();

    /* *
     * @brief设置音频音量
     *
     * @return
     */
    virtual int SetVolume(int vol);
    virtual int GetVolume();


    /* *
     * @brief设置音频播放采样率、位宽和单帧采样点数
     *
     * @return
     */
    virtual int SetProperty(const struct AudioProperties &properties);
    virtual int GetProperty(struct AudioProperties &properties);

    /* *
     * @brief 显示一张图片
     */
    virtual int Play();

    /* *
     * @brief 音频播放初始化
     */
    bool Init();
    bool Init(const std::string filePath);

private:
    AUDIO_INPUT_TYPE_E audioType;
};
} // namespace hilens
#endif // LIBHILESN_AUDIO_PLAY_H
