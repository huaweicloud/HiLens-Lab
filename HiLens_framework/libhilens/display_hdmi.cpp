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

#include "display_hdmi.h"
#include <securec.h>
#include "HiLensMedia.h"
#include "sfw_log.h"
#include "utils.h"
#include <opencv2/opencv.hpp>

#define ALIGN_MEMORY_WIDTH 32 /* mpp 宽度必须32位对齐 */

namespace hilens {
bool DisplayHDMI::envCreated = false;

HiLensEC DisplayHDMI::Show(const cv::Mat &frame)
{
    if (!envCreated) {
        SHOW_HDMI_PARAM params;
        alignWidth = (frame.cols + ALIGN_MEMORY_WIDTH - 1) / ALIGN_MEMORY_WIDTH * ALIGN_MEMORY_WIDTH;
        params.imageWidth = alignWidth;
        params.imageHeight = frame.rows * 2 / 3;
        params.outputSize = HILENS_VO_OUTPUT_1080P60;
        params.skillid = (char *)GetSkillID();
        int ret = CreateShowEnv(params);
        if (ret == HILENS_SUCCESS) {
            envCreated = true;
        } else if (ret == HILENS_RESOURCE_NOT_ENOUGH) {
            ERROR("HDMI resource is already occupied, failed to show HDMI!");
        } else {
            ERROR("HDMI resource init failed!");
            return UNKNOWN_ERROR;
        }
        if (alignWidth != frame.cols && ret == HILENS_SUCCESS) {
            showMat = cv::Mat(frame.rows, alignWidth, CV_8UC1);
            memset_s(showMat.data, (size_t)(params.imageWidth * params.imageHeight), 0,
                (size_t)(params.imageWidth * params.imageHeight));
            memset_s(showMat.data + params.imageWidth * params.imageHeight,
                (size_t)(params.imageWidth * params.imageHeight / 2), 128,
                (size_t)(params.imageWidth * params.imageHeight / 2));
            int left = ((alignWidth - frame.cols) / 2) / 2 * 2;
            showRect = cv::Rect(left, 0, frame.cols, frame.rows);
        }
    }
    int ret;
    if (alignWidth != frame.cols) {
        frame.copyTo(showMat(showRect));
        ret = ShowToLocalHDMI(showMat.data, showMat.total() * showMat.elemSize());
    } else {
        ret = ShowToLocalHDMI(frame.data, frame.total() * frame.elemSize());
    }

    return ret == 0 ? OK : UNKNOWN_ERROR;
}
} // namespace hilens