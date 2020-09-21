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

#include "local_camera.h"

#include <securec.h>
#include <HiLensMedia.h>
#include <unistd.h>
#include <stdexcept>

#include "utils.h"
#include "sfw_log.h"
#include "sf_common.h"

using namespace hilens;
using namespace cv;

bool LocalCamera::Init()
{
    if (!envCreated) {
        // 全局初始化
        CAMERA_PARAM params;
        // 先暂时固定为720p的分辨率
        params.imageHeight = CAM_HEIGHT;
        params.imageWidth = CAM_WIDTH;
        params.skillid = (char *)GetSkillID();
        params.fixelFormat = YVU_SEMIPLANAR_420;

        int ret = CreateCameraEnv(params);
        if (ret == HILENS_SUCCESS) {
            envCreated = true;
        } else if (ret == HILENS_RESOURCE_NOT_ENOUGH) {
            ERROR("local camera is already occupied!");
        } else {
            ERROR("local camera init failed!");
        }
    }

    return envCreated;
}

LocalCamera::~LocalCamera()
{
    if (envCreated) {
        DestroyCameraEnv();
        envCreated = false;
    }
}

int LocalCamera::Width()
{
    return CAM_WIDTH;
}

int LocalCamera::Height()
{
    return CAM_HEIGHT;
}

Mat LocalCamera::Read()
{
    // 没有调用init()的对象，不允许read
    if (!envCreated) {
        throw std::runtime_error("LocalCamera init failed!");
    }

    Mat frame(CAM_HEIGHT * 3 / 2, CAM_WIDTH, CV_8UC1);

    OUTPUT_PARAM outFrame = { 0 };
    while (ReadFrameFromCamera(&outFrame) != HILENS_SUCCESS) {
    }

    int ret = memcpy_s(frame.data, frame.total() * frame.elemSize(), outFrame.outData, outFrame.size);
    if (ret != 0) {
        ERROR("memcpy_s failed!");
    }

    SF_FREE(outFrame.outData);
    return frame;
}
