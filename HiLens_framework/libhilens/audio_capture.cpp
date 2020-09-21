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
#include "audio_capture.h"
#include "local_audio.h"
#include "file_audio.h"

using namespace std;

namespace hilens {
/* 构造AudioCapture 对象MIC */
shared_ptr<AudioCapture> AudioCapture::Create()
{
#ifdef CLOUD
    return nullptr;
#else
    LocalAudio *audio = new (std::nothrow) LocalAudio();
    if (audio) {
        if (!audio->Init()) {
            delete audio;
            return nullptr;
        }
    }
    return shared_ptr<AudioCapture>(audio);
#endif
}

/* 构造AudioCapture 对象音频文件 */
shared_ptr<AudioCapture> AudioCapture::Create(const std::string filePath)
{
#ifdef CLOUD
    FileAudio *audio = new (std::nothrow) FileAudio();
#else
    LocalAudio *audio = new (std::nothrow) LocalAudio();
#endif
    if (audio) {
        if (!audio->Init(filePath)) {
            delete audio;
            return nullptr;
        }
    }
    return shared_ptr<AudioCapture>(audio);
}
}
