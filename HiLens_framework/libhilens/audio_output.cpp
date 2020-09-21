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
#include <unistd.h>
#include <stdexcept>
#include <signal.h>
#include "utils.h"
#include "sf_common.h"
#include "audio_output.h"
#include "audio_play.h"
#include "errors.h"

typedef enum hiAIO_SOUND_MODE_E {
    AUDIO_SOUND_MODE_MONO = 0,   /* mono */
    AUDIO_SOUND_MODE_STEREO = 1, /* stereo */
    AUDIO_SOUND_MODE_BUTT
} AUDIO_SOUND_MODE_E;

static AUDIO_SOUND_MODE_E gSoundMode = AUDIO_SOUND_MODE_STEREO;
/* 申明内部处理函数 */
extern bool gAudioSystemInit;
extern bool AudioFileCheck(const std::string filePath, struct AudioProperties &property, int &chnCnt);
extern bool AudioSystemInit(void);
extern bool AudioSystemUnInit(void);


using namespace std;

namespace hilens {
/* 构造AudioOutput对象 */
shared_ptr<AudioOutput> AudioOutput::Create(const struct AudioProperties &property)
{
    AudioPlay *audioPlay = new (std::nothrow) AudioPlay();
    if (audioPlay) {
        audioPlay->SetProperty(property);
        if (!audioPlay->Init()) {
            delete audioPlay;
            return nullptr;
        }
    }
    return shared_ptr<AudioOutput>(audioPlay);
}

/* 构造AudioCapture 对象音频文件 */
shared_ptr<AudioOutput> AudioOutput::Create(const std::string filePath)
{
    AudioPlay *audioPlay = new (std::nothrow) AudioPlay();
    if (audioPlay) {
        if (!audioPlay->Init(filePath)) {
            delete audioPlay;
            return nullptr;
        }
    }
    return shared_ptr<AudioOutput>(audioPlay);
}

/* 解码播放aac格式音频文件 */
HiLensEC PlayAacFile(const std::string filePath, int vol)
{
    return OK;
}

void PlayHandleSigno(int signo) {}
}
