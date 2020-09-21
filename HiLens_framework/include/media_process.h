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

#ifndef HILENS_INCLUDE_MEDIA_PROCESS_H
#define HILENS_INCLUDE_MEDIA_PROCESS_H

#include <memory>
#include <opencv2/opencv.hpp>
#include "errors.h"

namespace hilens {
/* *
 * @brief 颜色转换码
 */
enum CvtCode {
    BGR2YUV_NV12,
    RGB2YUV_NV12,
    BGR2YUV_NV21,
    RGB2YUV_NV21
};

/* *
 * @brief 转换图片的颜色格式。opencv原生未提供RGB/BGR到NV12/NV21的转换选项，故在这里做补充
 *
 * @param src 源图
 * @param dst 目的图片
 * @param code 指定何种转换类型
 * @return HiLensEC 0为成功，其他为失败
 */
HiLensEC CvtColor(const cv::Mat &src, cv::Mat &dst, CvtCode code);

/* *
 * @brief 硬件加速的预处理器
 */
class Preprocessor {
public:
    /* *
     * @brief 构造并初始化一个3559加速的预处理器
     *
     * @return 成功则返回预处理器的指针，初始化失败则返回nullptr
     */
    static std::shared_ptr<Preprocessor> Create();

    /* *
     * @brief 缩放图片（硬件加速）
     * @param src 源图，必须为NV21的格式。宽度范围[64, 1920], 2的倍数；高度范围[64, 1080], 2的倍数。
     * @param dst 目的图片
     * @param w 缩放宽度，范围[64, 1920], 2的倍数
     * @param h 缩放高度，范围[64, 1080], 2的倍数
     * @param type 目的图片的格式，0为NV21,1为NV12
     * @return HiLensEC 0为成功，其他为失败
     */
    HiLensEC Resize(const cv::Mat &src, cv::Mat &dst, unsigned int w, unsigned int h, int type = 0);

    /* *
     * @brief 裁剪图片（硬件加速）
     * @param src 源图，必须为NV21的格式。宽度范围[64, 1920], 2的倍数；高度范围[64, 1080], 2的倍数。
     * @param dst 目的图片
     * @param x 裁剪区域左上角x坐标，范围[0, 1920], 2的倍数
     * @param y 裁剪区域左上角y坐标，范围[0, 1080], 2的倍数
     * @param w 裁剪宽度，范围[64, 1920], 2的倍数
     * @param h 裁剪高度，范围[64, 1080], 2的倍数
     * @param type 目的图片的格式，0为NV21,1为NV12
     * @return HiLensEC 0为成功，其他为失败
     */
    HiLensEC Crop(const cv::Mat &src, cv::Mat &dst, unsigned int x, unsigned int y, unsigned int w, unsigned int h,
        int type = 0);

    virtual ~Preprocessor();

protected:
    Preprocessor() {}
    Preprocessor(const Preprocessor &);
    bool Init();
    void *handle = NULL;
};
} // namespace hilens
#endif // HILENS_INCLUDE_MEDIA_PROCESS_H
