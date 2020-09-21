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
#include <signal.h>
#include "utils.h"
#include "sf_common.h"
#include "audio_play.h"

using namespace hilens;
using namespace std;

bool AudioPlay::Init()
{
    return true;
}

bool AudioPlay::Init(const std::string filePath)
{
    return true;
}

AudioPlay::~AudioPlay() {}

/* 播放MIC录入声音 */
int AudioPlay::Play()
{
    return 0;
}

/* 当前无用函数，防止python编译问题，后续统一删除 */
int AudioPlay::SetProperty(const struct AudioProperties &properties)
{
    return 0;
}

int AudioPlay::GetProperty(struct AudioProperties &properties)
{
    return 0;
}

int AudioPlay::SetVolume(int vol)
{
    return 0;
}

int AudioPlay::GetVolume()
{
    return 0;
}
