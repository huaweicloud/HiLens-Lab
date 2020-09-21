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

#include "mp4_reader.h"
#include "hilens_errorcode.h"
#include "sfw_log.h"
#include "utils.h"
#include <securec.h>
#include <unistd.h>
#include "sf_common.h"
#include <opencv2/opencv.hpp>

using namespace std;
using namespace hilens;

#define QUEUE_NUM 1

const int FRAME_SIZE = 1024 * 1024;

// MP4的H264码流格式由AVCC转换成Annex-B格式的名称
const string MP4_CONVERT_NAME = "h264_mp4toannexb";

// 默认(0, 0)意思即为使用原始视频帧的宽高
MP4Reader::MP4Reader() : MP4Reader(0, 0) {}

MP4Reader::MP4Reader(const unsigned int destWidth, const unsigned int destHeight)
    : stopFlag(false),
      pFormatCtx(nullptr),
      width(0),
      height(0),
      destWidth(destWidth),
      destHeight(destHeight),
      bsfContext(nullptr),
      isAnnexb(true)
{}

MP4Reader::~MP4Reader()
{
    stopFlag = true;
    imgqueue.ShutDown();
    if (decThread.joinable()) {
        decThread.join();
    }
    if (pFormatCtx) {
        avformat_close_input(&pFormatCtx);
        avformat_free_context(pFormatCtx);
    }
    if (bsfContext) {
        av_bsf_free(&bsfContext);
    }
}

int MP4Reader::Width()
{
    return width;
}

int MP4Reader::Height()
{
    return height;
}

// 依赖InitVideoContext()，里面会对width和height以及type进行初始化
bool MP4Reader::InitDecoder()
{
    // 默认使用原视频帧宽高
    if (destWidth == 0 && destHeight == 0) {
        destHeight = this->height;
        destWidth = this->width;
    }

    vDecoder = VDecFactory::Create(width, height, destWidth, destHeight, this->type, true);
    if (vDecoder == nullptr) {
        ERROR("MP4Reader failed to construct decode");
        return false;
    }
    vDecoder->RegisterCallback(vDecoder->BindCallback(&MP4Reader::decCallback, this));

    INFO("origin width(%d), height(%d), dest width(%d), height(%d)", width, height, destWidth, destHeight);

    this->frameSize = destHeight * destWidth * 3 / 2;
    return true;
}

bool MP4Reader::Init(const std::string &name)
{
    pFormatCtx = avformat_alloc_context();
    if (pFormatCtx == nullptr) {
        ERROR("avformat_alloc_context failed!");
        return false;
    }

    AVDictionary *options = NULL;
    av_dict_set(&options, "rtsp_transport", "tcp", 0);
    av_dict_set(&options, "stimeout", "3000000", 0);

    INFO("MP4Reader media file(%s)", name.c_str());

    int ret = avformat_open_input(&pFormatCtx, name.c_str(), 0, &options);
    if (ret < HILENS_OK) {
        char buf[256] = {0};
        av_strerror(ret, buf, sizeof(buf));
        av_dict_free(&options);
        ERROR("Couldn't open input stream, %s", buf);
        return false;
    }

    if (avformat_find_stream_info(pFormatCtx, NULL) < 0) {
        ERROR("Couldn't find stream information");
        return false;
    }

    INFO("--------------- File Information ----------------\n");
    av_dump_format(pFormatCtx, videoIndex, name.c_str(), 0);
    INFO("-------------------------------------------------\n");

    ret = InitVideoContext();
    if (ret != HILENS_OK) {
        return false;
    }

    // 如果不是Annex-B格式，则需要转换
    if (!isAnnexb) {
        ret = InitBitstreamFilter(pFormatCtx->streams[videoIndex]->codecpar);
        if (ret != HILENS_OK) {
            return false;
        }
    }

    imgqueue.SetCapacity(QUEUE_NUM);
    imgqueue.Start();

    // 依赖InitVideoContext()，要放到后面
    if (!InitDecoder()) {
        return false;
    }

    decThread = thread(&MP4Reader::decThreadFunc, this);

    return true;
}

int MP4Reader::InitVideoContext()
{
    if (pFormatCtx == nullptr) {
        ERROR("AVFormatContext not initialized!");
        return ERROR_GENERAL;
    }

    INFO("nb_streams = %d", pFormatCtx->nb_streams);

    for (int i = 0; i < pFormatCtx->nb_streams; ++i) {
        AVCodecParameters *codecpar = pFormatCtx->streams[i]->codecpar;
        INFO("i(%d), type(%d), id(%d)", i, codecpar->codec_type, codecpar->codec_id);

        if (codecpar->codec_type != AVMEDIA_TYPE_VIDEO) {
            continue;
        }

        videoIndex = i;
        char tmp[AV_FOURCC_MAX_STRING_SIZE] = {0};
        const char *result = av_fourcc_make_string(tmp, codecpar->codec_tag);
        if (result == nullptr) {
            continue;
        }

        isAnnexb = !(string("avc1") == result);
        if (isAnnexb) {
            INFO("mp4 file format: Annex-B");
        } else {
            INFO("mp4 file format: AVCC");
        }
    }

    if (videoIndex == -1) {
        ERROR("Failed to find video index\n");
        return ERROR_FFMPEG_AVFORMAT_INIT;
    }

    AVCodecParameters *codecpar = pFormatCtx->streams[videoIndex]->codecpar;
    INFO("The avcodec name is %s, format=%d, bit_rate=%ld", avcodec_get_name(codecpar->codec_id), codecpar->format,
        codecpar->bit_rate);

    switch (codecpar->codec_id) {
        case AV_CODEC_ID_H264:
            this->type = VDecFactory::DecH264;
            break;
        case AV_CODEC_ID_H265:
            this->type = VDecFactory::DecH265;
            break;
        default:
            ERROR("Video stream codec not supported!");
            return ERROR_FFMPEG_AVFORMAT_INIT;
    }

    // 读取图片宽高数据
    height = codecpar->height;
    width = codecpar->width;

    return HILENS_OK;
}

int MP4Reader::InitBitstreamFilter(const AVCodecParameters *codecpar)
{
    int ret = 0;

    const AVBitStreamFilter *filter = av_bsf_get_by_name(MP4_CONVERT_NAME.c_str());
    if (filter == nullptr) {
        ERROR("Unknow bitstream filter.");
        return ERROR_FFMPEG_BITSTREAMFILTER;
    }

    ret = av_bsf_alloc(filter, &bsfContext);
    if (ret < 0 || bsfContext == nullptr) {
        ERROR("av_bsf_alloc failed, ret=(%d)", ret);
        return ERROR_FFMPEG_BITSTREAMFILTER;
    }

    if ((ret = avcodec_parameters_copy(bsfContext->par_in, codecpar)) < 0) {
        ERROR("avcodec_parameters_copy failed, ret=%d", ret);
        return ERROR_FFMPEG_BITSTREAMFILTER;
    }

    if (ret = av_bsf_init(bsfContext) < 0) {
        ERROR("av_bsf_init failed, ret=%d", ret);
        return ERROR_FFMPEG_BITSTREAMFILTER;
    }

    return HILENS_OK;
}

int MP4Reader::FilterStream(AVPacket *srcPkt, shared_ptr<uint8_t> &tmpFrame, unsigned int &tmpFrameSize)
{
    if (isAnnexb) {
        memcpy_s(tmpFrame.get(), srcPkt->size, srcPkt->data, srcPkt->size);
        tmpFrameSize = srcPkt->size;
        return HILENS_OK;
    }

    // 非Annex-B格式，则需要转换
    int ret = av_bsf_send_packet(bsfContext, srcPkt);
    if (ret < 0) {
        ERROR("av_bsf_send_packet failed, ret=%d", ret);
        return ret;
    }

    // 这个地方receive的packet对象最好是srcPkt，里面会应用srcPkt的参数
    while ((ret = av_bsf_receive_packet(bsfContext, srcPkt) == 0)) {
        if (srcPkt->size > 0) {
            memcpy_s(tmpFrame.get() + tmpFrameSize, srcPkt->size, srcPkt->data, srcPkt->size);
            tmpFrameSize += srcPkt->size;
        } else {
            ERROR("av_bsf_receive_packet failed!");
            return ERROR_GENERAL;
        }

        av_packet_unref(srcPkt);
    }

    if (ret == AVERROR(EAGAIN)) {
        return HILENS_OK;
    }

    return ret;
}

void MP4Reader::decCallback(cv::Mat &outimg)
{
    if (outimg.data) {
        imgqueue.Push(outimg); // 这里必须卡住，mp4和rtsp是不一样的
    } else {
        outimg = cv::Mat(destHeight * 3 / 2, destWidth, CV_8UC1);
    }
}

void MP4Reader::decThreadFunc()
{
    const unsigned int maxFrameSize = 1024 * 1024;
    shared_ptr<uint8_t> tmpFrame(new uint8_t[maxFrameSize]);

    while (!stopFlag.load()) {
        Decode(tmpFrame);
    }
    imgqueue.ShutDown();
}

int MP4Reader::Decode(std::shared_ptr<uint8_t> &tmpFrame)
{
    AVPacket pkt;
    av_init_packet(&pkt);
    int ret = HILENS_OK;

    unsigned int tmpFrameSize = 0;
    if (tmpFrame == nullptr) {
        ERROR("MP4Reader tmpFrame memory error.");
        return ERROR_MEMORY_ALLOC;
    }

    ret = av_read_frame(pFormatCtx, &pkt);
    if (ret < 0) {
        // 非Annex-B格式，需要处理缓存中的数据
        if (!isAnnexb) {
            ret = FilterStream(nullptr, tmpFrame, tmpFrameSize);
            stopFlag = true;
            return ret;
        } else {
            WARN("Read frame failed!\n");
            stopFlag = true;
            return ret;
        }
    } else if (pkt.stream_index == videoIndex) {
        if (pkt.data == nullptr || pkt.size <= 0) {
            ERROR("newPkt data nullptr, (%d)", pkt.size);
            av_packet_unref(&pkt);
            return ERROR_MEMORY_ALLOC;
        }

        ret = FilterStream(&pkt, tmpFrame, tmpFrameSize);
        if (ret != HILENS_OK) {
            av_packet_unref(&pkt);
            return ERROR_MEMORY_ALLOC;
        }

        ret = vDecoder->DecodeFrameBuf(tmpFrame.get(), tmpFrameSize);
        if (HILENS_OK != ret) {
            WARN("Failed to decode video stream");
            av_packet_unref(&pkt);
            return ret;
        }
    } else {
        // 其他stream_index先忽略
        av_packet_unref(&pkt);
        return ERROR_GENERAL;
    }

    av_packet_unref(&pkt);

    return HILENS_OK;
}

cv::Mat MP4Reader::Read()
{
    cv::Mat retMat;
    imgqueue.Pop(retMat);

    if (stopFlag.load() || !retMat.data) {
        throw std::length_error("MP4Reader read finished!");
    }

    return retMat;
}
