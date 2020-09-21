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

#ifndef LIBHILENS_MP4_READER_H
#define LIBHILENS_MP4_READER_H

#include <string>
#include <memory>
#include <opencv2/opencv.hpp>
#include "license_video_capture.h"
#include "vdecfactory.h"
#include <thread>
#include <mutex>
#include "BlockingQueue.h"

extern "C" {
#include <libavformat/avformat.h>
}

namespace hilens {
class MP4Reader : public LicenseVideoCapture {
public:
    MP4Reader();
    MP4Reader(const unsigned int destWidth, const unsigned int destHeight);
    ~MP4Reader();

    virtual bool Init(const std::string &name);
    virtual cv::Mat Read();
    virtual int Width();
    virtual int Height();

private:
    int Decode(std::shared_ptr<uint8_t> &tmpFrame);
    int InitBitstreamFilter(const AVCodecParameters *codecpar);
    int InitVideoContext();
    bool InitDecoder();
    int FilterStream(AVPacket *srcPkt, std::shared_ptr<uint8_t> &tmpFrame, unsigned int &tmpFrameSize);
    void decCallback(cv::Mat &outimg);

private:
    VDecFactory::DecType type;

    IVDec vDecoder;
    size_t frameSize;

    int videoIndex;
    std::thread readThread;
    std::atomic_bool stopFlag;
    bool isAnnexb;

    AVFormatContext *pFormatCtx;
    AVBSFContext *bsfContext;

    int width;
    int height;
    int destWidth;
    int destHeight;
    /* add to potect read when async */
    std::thread decThread;
    void decThreadFunc();
    BlockingQueue<cv::Mat> imgqueue;
};
} // namespace hilens
#endif // LIBHILENS_MP4_READER_H