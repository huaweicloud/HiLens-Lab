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

#include "wrapper.h"
#include <sstream>
#include <securec.h>
#include <sfw_log.h>

using namespace std;
using namespace cv;
using namespace hilens;

bool VideoCaptureWrapper::Init()
{
    impl = VideoCapture::Create();
    return impl != nullptr;
}

bool VideoCaptureWrapper::Init(const string &name)
{
    impl = VideoCapture::Create(name);
    return impl != nullptr;
}

bool VideoCaptureWrapper::Init(const string &name, const unsigned int width, const unsigned int height)
{
    impl = VideoCapture::Create(name, width, height);
    return impl != nullptr;
}

bool VideoCaptureWrapper::Init(int dev)
{
    impl = VideoCapture::Create(dev);
    return impl != nullptr;
}

void VideoCaptureWrapper::ReadArray(unsigned char *data, unsigned int size)
{
    readError = false;

    if (nullptr == impl) {
        ERROR("VideoCapture impl is nullptr");
        readError = true;
        return;
    }

    cv::Mat frame;
    try {
        frame = impl->Read();
    } catch (const std::runtime_error &e) {
        ERROR("Failed to read frame: %s", e.what());
        impl.reset();
        readError = true;
        return;
    } catch (const std::length_error &e) {
        // MP4文件读取，会抛出这个异常，表示文件读取结束，打印日志用INFO级别
        INFO("%s", e.what());
        impl.reset();
        readError = true;
        return;
    }

    if (0 != memcpy_s(data, size, frame.data, size)) {
        ERROR("Failed to copy memory");
        readError = true;
    }
}

bool VideoCaptureWrapper::ReadError()
{
    return readError;
}

int VideoCaptureWrapper::Width()
{
    if (!impl) {
        ERROR("VideoCapture impl is nullptr");
        return 0;
    }
    return impl->Width();
}

int VideoCaptureWrapper::Height()
{
    if (!impl) {
        ERROR("VideoCapture impl is nullptr");
        return 0;
    }
    return impl->Height();
}

int hilens::CvtColorWrapper(const unsigned char *srcData, unsigned int srcSize, unsigned char *dstData,
    unsigned int dstSize, unsigned int rows, unsigned int cols, CvtCode code)
{
    Mat srcMat(rows, cols, CV_8UC3);
    if (0 != memcpy_s(srcMat.data, srcMat.total() * srcMat.elemSize(), srcData, srcSize)) {
        ERROR("Failed to copy memory");
        return UNKNOWN_ERROR;
    }
    Mat dstMat;
    CvtColor(srcMat, dstMat, code);
    if (0 != memcpy_s(dstData, dstSize, dstMat.data, dstMat.total() * dstMat.elemSize())) {
        ERROR("Failed to copy memory");
        return UNKNOWN_ERROR;
    }
    return 0;
}

int PreprocessorWrapper::ResizeArray(const unsigned char *srcData, unsigned int srcSize, unsigned char *dstData,
    unsigned int dstSize, unsigned int cols, unsigned int rows, unsigned int w, unsigned int h, int type)
{
    Mat srcMat(rows, cols, CV_8UC1);
    memcpy_s(srcMat.data, srcMat.total() * srcMat.elemSize(), srcData, srcSize);
    Mat dstMat;
    HiLensEC ec = OK;
    if (OK != (ec = Resize(srcMat, dstMat, w, h, type))) {
        return ec;
    }
    memcpy_s(dstData, dstSize, dstMat.data, dstMat.total() * dstMat.elemSize());
    return OK;
}
int PreprocessorWrapper::CropArray(const unsigned char *srcData, unsigned int srcSize, unsigned char *dstData,
    unsigned int dstSize, unsigned int cols, unsigned int rows, unsigned int x, unsigned int y, unsigned int w,
    unsigned int h, int type)
{
    Mat srcMat(rows, cols, CV_8UC1);
    memcpy_s(srcMat.data, srcMat.total() * srcMat.elemSize(), srcData, srcSize);
    Mat dstMat;
    HiLensEC ec = OK;
    if (OK != (ec = Crop(srcMat, dstMat, x, y, w, h, type))) {
        return ec;
    }
    memcpy_s(dstData, dstSize, dstMat.data, dstMat.total() * dstMat.elemSize());
    return OK;
}

void InferDataWrapper::ToArrayUint8(unsigned char *data, unsigned int size)
{
    memcpy_s(data, size, this->data.get(), this->size);
}

void InferDataWrapper::ToArrayFloat(float *data, unsigned int size)
{
    memcpy_s(data, size * sizeof(float), this->data.get(), this->size);
}

bool ModelWrapper::Init(const std::string &filename)
{
    impl = Model::Create(filename);
    return impl != nullptr;
}

int ModelWrapper::InferWrapper(const std::vector<hilens::InferDataWrapper> &inputs,
    std::vector<hilens::InferDataWrapper> &outputs)
{
    if (!impl) {
        ERROR("Model impl is nullptr");
        return UNKNOWN_ERROR;
    }

    InferDataVec realInputs;
    InferDataVec realOutputs;
    for (auto it = inputs.begin(); it != inputs.end(); ++it) {
        realInputs.push_back(*it);
    }
    int ret = impl->Infer(realInputs, realOutputs);
    for (auto it = realOutputs.begin(); it != realOutputs.end(); ++it) {
        InferDataWrapper o;
        o.data = it->data;
        o.size = it->size;
        outputs.push_back(o);
    }
    return ret;
}

bool DisplayWrapper::Init(Display::Type type, const char *path)
{
    impl = Display::Create(type, path);
    return impl != nullptr;
}

int DisplayWrapper::ShowArray(const unsigned char *srcData, unsigned int srcSize, unsigned int cols, unsigned int rows)
{
    if (!impl) {
        ERROR("Display impl is nullptr");
        return UNKNOWN_ERROR;
    }

    Mat frame(rows, cols, CV_8UC1);
    memcpy_s(frame.data, frame.total() * frame.elemSize(), srcData, srcSize);
    return impl->Show(frame);
}

std::string hilens::GetSkillConfigText()
{
    Json::Value cfg = GetSkillConfig();
    std::stringstream ss;
    ss << cfg;
    return ss.str();
}

bool AudioCaptureWrapper::Init()
{
    impl = AudioCapture::Create();
    return impl != nullptr;
}

bool AudioCaptureWrapper::Init(const std::string &filePath)
{
    impl = AudioCapture::Create(filePath);
    return impl != nullptr;
}

int AudioCaptureWrapper::ReadArray(int numFrames)
{
    if (nullptr == impl) {
        ERROR("AudioCapture impl is nullptr");
        return UNKNOWN_ERROR;
    }
    if (numFrames <= 0 || numFrames > MAX_FRAME_NUM) {
        ERROR("Failed to read frame: Number of frame(s) should range from 1 to %d.", MAX_FRAME_NUM);
        return UNKNOWN_ERROR;
    }

    totalSize = 0;
    int ret = impl->Read(frames, numFrames);
    if (0 != ret) {
        return ret;
    }
    totalSize = frames.size;
    return 0;
}

void AudioCaptureWrapper::ToNumpyArray(unsigned char *data, unsigned int size)
{
    memcpy_s(data, frames.size, frames.data.get(), frames.size);
}

int AudioCaptureWrapper::SetVolume(int vol)
{
    if (nullptr == impl) {
        ERROR("AudioCapture impl is nullptr");
        return UNKNOWN_ERROR;
    }
    return impl->SetVolume(vol);
}

int AudioCaptureWrapper::GetVolume()
{
    if (nullptr == impl) {
        ERROR("AudioCapture impl is nullptr");
        return UNKNOWN_ERROR;
    }
    return impl->GetVolume();
}

bool AudioOutputWrapper::Init(const std::string &filePath)
{
    impl = AudioOutput::Create(filePath);
    return impl != nullptr;
}

int AudioOutputWrapper::Play()
{
    if (nullptr == impl) {
        ERROR("AudioOutput impl is nullptr");
        return UNKNOWN_ERROR;
    }
    return impl->Play();
}

HiLensEC AudioOutputWrapper::PlayAacFile(const std::string filePath, int vol)
{
    return hilens::PlayAacFile(filePath, vol);
}
