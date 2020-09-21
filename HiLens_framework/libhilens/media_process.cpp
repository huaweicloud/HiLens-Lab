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

#include "media_process.h"
#include <unistd.h>
#include <securec.h>
#include <HiLensMedia.h>
#include "sfw_log.h"
#include "utils.h"

using namespace std;
using namespace cv;
using namespace hilens;

#define ALIGN_MEMORY_WIDTH 32

struct MutexHdl {
    void *hdl;
    mutex mtx;
};

inline bool CheckSizeInvalid(unsigned int width, unsigned int height)
{
    int minSize = 64;
    return (width < minSize || width > getMaxWidth() || height < minSize || height > getMaxHeight() || width % 2 != 0 ||
        height % 2 != 0);
}

HiLensEC hilens::CvtColor(const Mat &src, Mat &dst, CvtCode code)
{
    int w = src.cols;
    int h = src.rows;

    // 先转RGB/BGR到YUV_I420
    switch (code) {
        case RGB2YUV_NV12:
        case RGB2YUV_NV21:
            cvtColor(src, dst, CV_RGB2YUV_I420);
            break;
        case BGR2YUV_NV12:
        case BGR2YUV_NV21:
            cvtColor(src, dst, CV_BGR2YUV_I420);
            break;
    }

    // 再从YUV_I420手动转为NV12/NV21
    int offset = w * h;
    int len = w * h / 2;
    int halflen = len / 2;
    unsigned char tmp[len];
    unsigned char *data = dst.data;
    int ret = memcpy_s(tmp, len, data + offset, len);
    if (ret != 0) {
        ERROR("CvtColor memcpy_s failed!");
        return UNKNOWN_ERROR;
    }
    switch (code) {
        case RGB2YUV_NV12:
        case BGR2YUV_NV12:
            for (int i = 0; i < halflen; ++i) {
                data[offset + i * 2] = tmp[i];
                data[offset + i * 2 + 1] = tmp[halflen + i];
            }
            break;
        case RGB2YUV_NV21:
        case BGR2YUV_NV21:
            for (int i = 0; i < halflen; ++i) {
                data[offset + i * 2] = tmp[halflen + i];
                data[offset + i * 2 + 1] = tmp[i];
            }
            break;
    }

    return OK;
}

shared_ptr<Preprocessor> Preprocessor::Create()
{
    Preprocessor *proc = new (std::nothrow) Preprocessor();
    if (proc) {
        if (!proc->Init()) {
            delete proc;
            return nullptr;
        }
    }
    return shared_ptr<Preprocessor>(proc);
}

bool Preprocessor::Init()
{
    handle = (void *)(new MutexHdl);
    ((MutexHdl *)handle)->hdl = CreateProcessEnv(GetSkillID());
    return ((MutexHdl *)handle)->hdl != NULL;
}

Preprocessor::~Preprocessor()
{
    if (handle) {
        DestroyProcessEnv(((MutexHdl *)handle)->hdl);
        delete (MutexHdl *)handle;
        handle = NULL;
    }
}

HiLensEC Preprocessor::Resize(const Mat &src, Mat &dst, unsigned int w, unsigned int h, int type)
{
    unsigned int width = src.cols;
    unsigned int height = src.rows * 2 / 3;

    // 参数合法性检查
    if (CheckSizeInvalid(width, height)) {
        ERROR("Resize abort: invalid src size: src width=%d, src height=%d", width, height);
        return INVALID_SRC_SIZE;
    }
    if (CheckSizeInvalid(w, h)) {
        ERROR("Resize abort: invalid output size: src width=%d, src height=%d", w, h);
        return INVALID_DST_SIZE;
    }

    std::lock_guard<std::mutex> lk(((MutexHdl *)handle)->mtx);
    PROCESS_PARAM params;
    params.srcWidth = width;
    params.srcHeight = height;
    params.destWidth = w;
    params.destHeight = h;
    params.fixelFormat = type == 0 ? YVU_SEMIPLANAR_420 : YUV_SEMIPLANAR_420;
    params.imageBuf = src.data;
    dst = Mat(h * 3 / 2, w, CV_8UC1);
    OUTPUT_PARAM outParams;
    outParams.outData = dst.data;
    outParams.size = w * h * 3 / 2;
    outParams.allignwidth = (w + ALIGN_MEMORY_WIDTH - 1) / ALIGN_MEMORY_WIDTH * ALIGN_MEMORY_WIDTH;
    outParams.destwidth = w;
    outParams.height = h;

    if (ResizeImage(((MutexHdl *)handle)->hdl, &params, &outParams) != HILENS_SUCCESS) {
        ERROR("mpp ResizeImage failed!");
        return MPP_PROCESS_FAILED;
    }

    return OK;
}

HiLensEC CropCheck(unsigned int width, unsigned int height, unsigned int x, unsigned int y, unsigned int w,
    unsigned int h)
{
    if (CheckSizeInvalid(width, height)) {
        ERROR("Crop abort: invalid src size: src width=%d, src height=%d", width, height);
        return INVALID_SRC_SIZE;
    }
    if (CheckSizeInvalid(w, h)) {
        ERROR("Crop abort: invalid output size: src width=%d, src height=%d", w, h);
        return INVALID_DST_SIZE;
    }
    if (x > width || y > height || x % 2 != 0 || y % 2 != 0) {
        ERROR("Crop abort: invalid output size: x=%d, y=%d", w, h);
        return INVALID_DST_SIZE;
    }
    return OK;
}

HiLensEC Preprocessor::Crop(const Mat &src, Mat &dst, unsigned int x, unsigned int y, unsigned int w, unsigned int h,
    int type)
{
    unsigned int width = src.cols;
    unsigned int height = src.rows * 2 / 3;

    // 参数合法性检查
    HiLensEC ret = CropCheck(width, height, x, y, w, h);
    if (ret != OK) {
        return ret;
    }

    PROCESS_PARAM params;
    params.srcWidth = width;
    params.srcHeight = height;
    params.startX = x;
    params.startY = y;
    params.destWidth = w;
    params.destHeight = h;
    params.fixelFormat = type == 0 ? YVU_SEMIPLANAR_420 : YUV_SEMIPLANAR_420;
    params.imageBuf = src.data;
    OUTPUT_PARAM outParams = { 0 };
    if (CropImage(((MutexHdl *)handle)->hdl, &params, &outParams) != HILENS_SUCCESS) {
        ERROR("mpp CropImage failed");
        return MPP_PROCESS_FAILED;
    }

    // mpp 出来的图宽度有32对齐的padding
    if (w % 32 == 0) {
        dst = Mat(h * 3 / 2, w, CV_8UC1);
        int ret = memcpy_s(dst.data, dst.total() * dst.elemSize(), outParams.outData, outParams.size);
        free(outParams.outData);
        if (ret != 0) {
            ERROR("CropImage memcpy_s failed1");
            return UNKNOWN_ERROR;
        }
    } else {
        // 对于不满足宽度32对齐的
        unsigned int padded = (w / 32 + 1) * 32;
        Mat tmp = Mat(h * 3 / 2, padded, CV_8UC1);
        int ret = memcpy_s(tmp.data, tmp.total() * tmp.elemSize(), outParams.outData, outParams.size);
        free(outParams.outData);
        if (ret != 0) {
            ERROR("CropImage memcpy_s failed2");
            return UNKNOWN_ERROR;
        }
        Mat roi(tmp, Rect(0, 0, w, h * 3 / 2));
        roi.copyTo(dst);
    }

    return OK;
}