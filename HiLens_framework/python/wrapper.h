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

#ifndef HILENS_PYTHON_WRAPPER_H
#define HILENS_PYTHON_WRAPPER_H

#include <Python.h>
#include "hilens.h"

namespace hilens {
// 摄像头
class VideoCaptureWrapper {
public:
    VideoCaptureWrapper() {}
    ~VideoCaptureWrapper() {}
    bool Init();
    bool Init(const std::string &name);
    bool Init(const std::string &name, const unsigned int width, const unsigned int height);
    bool Init(int dev);

    void ReadArray(unsigned char *data, unsigned int size);
    // 由于尝试将C++的异常通过swig抛出到Python中没有成功，这里加一个接口来指示读取是否成功。
    bool ReadError();
    int Width();
    int Height();

private:
    std::shared_ptr<VideoCapture> impl = nullptr;
    bool readError = false;
};

/* *
 * @brief 颜色转换（外壳）
 * @param srcData 源数组指针
 * @param srcSize 源数组大小
 * @param dstData 目标数组指针
 * @param dstSize 目标数组大小
 * @param rows 源图行数
 * @param cols 源图列数
 * @param code 转换码
 */
int CvtColorWrapper(const unsigned char *srcData, unsigned int srcSize, unsigned char *dstData, unsigned int dstSize,
    unsigned int rows, unsigned int cols, CvtCode code);

class PreprocessorWrapper : public Preprocessor {
public:
    PreprocessorWrapper() {}
    ~PreprocessorWrapper() {}
    /* *
     * @brief 缩放
     * @param srcData 源数组指针
     * @param srcSize 源数组大小
     * @param dstData 目标数组指针
     * @param dstSize 目标数组大小
     * @param cols 源图列数
     * @param rows 源图行数
     * @param w 缩放后宽度（像素）
     * @param h 缩放后高度（像素）
     * @param type 目的图片格式
     */
    int ResizeArray(const unsigned char *srcData, unsigned int srcSize, unsigned char *dstData, unsigned int dstSize,
        unsigned int cols, unsigned int rows, unsigned int w, unsigned int h, int type = 0);

    /* *
     * @brief 裁剪
     * @param srcData 源数组指针
     * @param srcSize 源数组大小
     * @param dstData 目标数组指针
     * @param dstSize 目标数组大小
     * @param cols 源图列数
     * @param rows 源图行数
     * @param x 裁剪左上角x坐标
     * @param y 裁剪左上角y坐标
     * @param w 裁剪后宽度（像素）
     * @param h 裁剪后高度（像素）
     * @param type 目的图片格式
     */
    int CropArray(const unsigned char *srcData, unsigned int srcSize, unsigned char *dstData, unsigned int dstSize,
        unsigned int cols, unsigned int rows, unsigned int x, unsigned int y, unsigned int w, unsigned int h,
        int type = 0);

    virtual bool Init()
    {
        return Preprocessor::Init();
    }
};

class InferDataWrapper : public InferData {
public:
    InferDataWrapper() : InferData() {}
    InferDataWrapper(const unsigned char *data, unsigned int size) : InferData(data, size) {}
    // 由于swig在重载时无法区分const unsigned char *和const float *data，故在后面加一个int以作区分，否则无法正常重载
    InferDataWrapper(const float *data, unsigned int size, int) : InferData((const unsigned char *)data, size * 4) {}
    ~InferDataWrapper() {}
    void ToArrayUint8(unsigned char *data, unsigned int size);
    void ToArrayFloat(float *data, unsigned int size);
};

class ModelWrapper {
public:
    ModelWrapper() {}
    ~ModelWrapper() {}
    bool Init(const std::string &filename);
    int InferWrapper(const std::vector<hilens::InferDataWrapper> &inputs,
        std::vector<hilens::InferDataWrapper> &outputs);

private:
    std::shared_ptr<Model> impl = nullptr;
};

class DisplayWrapper {
public:
    DisplayWrapper() {}
    ~DisplayWrapper() {}
    bool Init(Display::Type type, const char *path = NULL);
    int ShowArray(const unsigned char *srcData, unsigned int srcSize, unsigned int cols, unsigned int rows);

private:
    std::shared_ptr<Display> impl = nullptr;
};

std::string GetSkillConfigText();

// MIC数据读取的python接口包装类
class AudioCaptureWrapper {
public:
    AudioCaptureWrapper() {}
    ~AudioCaptureWrapper() {}
    bool Init();
    bool Init(const std::string &filePath);
    int SetVolume(int vol);
    int GetVolume();
    int ReadArray(int numFrames);
    void ToNumpyArray(unsigned char *data, unsigned int size);

    int totalSize;
    enum class SAMPLE_RATE {
        AUDIO_SAMPLE_RATE_8000 = 8000,   /* 8K samplerate */
        AUDIO_SAMPLE_RATE_12000 = 12000, /* 12K samplerate */
        AUDIO_SAMPLE_RATE_11025 = 11025, /* 11.025K samplerate */
        AUDIO_SAMPLE_RATE_16000 = 16000, /* 16K samplerate */
        AUDIO_SAMPLE_RATE_22050 = 22050, /* 22.050K samplerate */
        AUDIO_SAMPLE_RATE_24000 = 24000, /* 24K samplerate */
        AUDIO_SAMPLE_RATE_32000 = 32000, /* 32K samplerate */
        AUDIO_SAMPLE_RATE_44100 = 44100, /* 44.1K samplerate */
        AUDIO_SAMPLE_RATE_48000 = 48000, /* 48K samplerate */
        AUDIO_SAMPLE_RATE_64000 = 64000, /* 64K samplerate */
        AUDIO_SAMPLE_RATE_96000 = 96000, /* 96K samplerate */
    };
    enum class BIT_WIDTH {
        AUDIO_BIT_WIDTH_16 = 1 /* 16bit width */
    };
    enum class NUM_SAMPLES_PER_FRAME {
        MIN_SAMPLES = 80,   /* 每帧音频采样点数的最小值 */
        MAX_SAMPLES = 2048, /* 每音频帧采样点数的最大值 */
    };

private:
    std::shared_ptr<AudioCapture> impl = nullptr;
    // 最多一次能读512帧（默认音频参数下大约12s时长）
    const int MAX_FRAME_NUM = 512;
    AudioFrame frames;
};

class AudioOutputWrapper {
public:
    AudioOutputWrapper() {}
    ~AudioOutputWrapper() {}
    bool Init(const std::string &filePath);
    int Play();
    HiLensEC PlayAacFile(const std::string filePath, int vol);

private:
    std::shared_ptr<AudioOutput> impl = nullptr;
};
} // namespace hilens
#endif // HILENS_PYTHON_WRAPPER_H
