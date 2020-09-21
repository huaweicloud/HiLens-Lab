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

#ifndef HILENS_ERROR_CODE_H
#define HILENS_ERROR_CODE_H

namespace hilens {
const int HILENS_OK = 0;

// common错误码
const int ERROR_GENERAL = 1; // 通用错误
const int ERROR_READ_FILE_FAILED = 2;
const int ERROR_MEMORY_ALLOC = 3;
const int ERROR_FSEEK_FILE = 4;

// ffmpeg错误码
const int ERROR_FFMPEG_AVFORMAT_INIT = 101;
const int ERROR_FFMPEG_BITSTREAMFILTER = 102;

// mp4 reader错误码
const int ERROR_DECODE_FRAME_SIZE = 201;
} // namespace hilens

#endif