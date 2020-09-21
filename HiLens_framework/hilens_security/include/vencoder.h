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

#ifndef HILENS_SECURITY_VENCODER_H
#define HILENS_SECURITY_VENCODER_H

#include <HiLensMedia.h>

namespace hilens {
class VEncoder {
public:
    VEncoder();
    /* *
     * @brief 初始化视频编码器
     *
     * @param w 视频宽度（像素）
     * @param h 视频高度（像素）
     * @return int 初始化结果
     */
    int Init(int w, int h);
    virtual ~VEncoder();
    /* *
     * @brief 编码一帧视频
     *
     * @param indata 输入视频数据
     * @param insize 输入视频数据大小（字节）
     * @param outdata 输出视频数据
     * @param outsize 输出视频数据大小（字节）
     * @return int 编码结果
     */
    int EncodeFrame(const unsigned char *indata, unsigned int insize, unsigned char **outdata, unsigned int *outsize);

private:
    MediaHandle handle;
};
} // namespace hilens
#endif // HILENS_SECURITY_VENCODER_H