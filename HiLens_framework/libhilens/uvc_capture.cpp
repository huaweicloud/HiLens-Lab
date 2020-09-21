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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <getopt.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <malloc.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <sys/ioctl.h>

#include <asm/types.h>
#include <linux/videodev2.h>
#include <opencv2/opencv.hpp>
#include <securec.h>

#include "uvc_capture.h"
#include "sfw_log.h"

using namespace hilens;
using namespace std;
using namespace cv;

/* 定义模块名便于日志定位 */
#define UVC_MODULE_NAME "UVC Camera"

/* 分辨率 */
#define UVC_VIDEO_WIDTH 1280
#define UVC_VIDEO_HEIGHT 720
/* 视频格式，只支持YUYV(帧率低，不超过10fps)和MJPEG(帧率高，默认30fps)两种 */
#define UVC_VIDEO_FORMAT V4L2_PIX_FMT_MJPEG
/* V4L2缓存个数，不低于3个，不超过5个 */
#define UVC_BUFFER_COUNT 4
/* 最多支持两路USB摄像头 */
#define UVC_CAMERA_DEVICE_NUM 2
/* UVC免驱动摄像头生成的设备节点 */
#define UVC_CAMERA_DEVICE0 "/dev/video0"
#define UVC_CAMERA_DEVICE1 "/dev/video2"
/* 根据设备ID选择对应的设备节点 */
static const char *uvcCameraDevice[UVC_CAMERA_DEVICE_NUM] = {UVC_CAMERA_DEVICE0, UVC_CAMERA_DEVICE1};

bool UVCCapture::Check(int dev)
{
    /* 入参检查 */
    devId = dev;
    if (devId < 0 || devId > 1) {
        ERROR("[%s]Device ID(%d) not exists!\n", UVC_MODULE_NAME, devId);
        return false;
    }

    /* 1、打开视频设备文件 */
    fd = open(uvcCameraDevice[devId], O_RDWR, 0);
    if (fd < 0) {
        ERROR("[%s]Open %s failed!\n", UVC_MODULE_NAME, uvcCameraDevice[devId]);
        return false;
    }

    /* 2、查询视频设备能力 */
    struct v4l2_capability cap;
    int ret = ioctl(fd, VIDIOC_QUERYCAP, &cap);
    if (ret < 0) {
        ERROR("[%s]VIDIOC_QUERYCAP failed (%d)\n", UVC_MODULE_NAME, ret);
        return false;
    }

    if (!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE)) {
        ERROR("[%s]dev%d not supports video capturing!\n", UVC_MODULE_NAME, devId);
        return false;
    }
    return true;
}

bool UVCCapture::SetParam()
{
    struct v4l2_format fmt;
    int ret = memset_s(&fmt, sizeof(fmt), 0, sizeof(fmt));
    if (ret != 0) {
        ERROR("[%s]memset_s fmt failed (%d)\n", UVC_MODULE_NAME, ret);
        return false;
    }
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt.fmt.pix.width = UVC_VIDEO_WIDTH;
    fmt.fmt.pix.height = UVC_VIDEO_HEIGHT;
    fmt.fmt.pix.pixelformat = UVC_VIDEO_FORMAT;
    fmt.fmt.pix.field = V4L2_FIELD_INTERLACED;
    ret = ioctl(fd, VIDIOC_S_FMT, &fmt);
    if (ret < 0) {
        ERROR("[%d]VIDIOC_S_FMT failed (%d)\n", UVC_MODULE_NAME, ret);
        return false;
    }

    /* 如果该视频设备驱动不支持你所设定的图像格式，视频驱动会重新修改struct
     * v4l2_format结构体变量的值为该视频设备所支持的图像格式，设置完格式后，获取实际视频格式 */
    ret = ioctl(fd, VIDIOC_G_FMT, &fmt);
    if (ret < 0) {
        ERROR("[%s]VIDIOC_G_FMT failed (%d)\n", UVC_MODULE_NAME, ret);
        return false;
    }

    width = fmt.fmt.pix.width;
    height = fmt.fmt.pix.height;
    readFrameNum = 0;
    return true;
}

bool UVCCapture::MallocBuffer()
{
    struct v4l2_requestbuffers reqbuf;
    int ret = memset_s(&reqbuf, sizeof(reqbuf), 0, sizeof(reqbuf));
    if (ret != 0) {
        ERROR("[%s]memset_s reqbuf failed (%d)\n", UVC_MODULE_NAME, ret);
        return false;
    }
    reqbuf.count = UVC_BUFFER_COUNT;
    reqbuf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    reqbuf.memory = V4L2_MEMORY_MMAP;
    ret = ioctl(fd, VIDIOC_REQBUFS, &reqbuf);
    if (ret < 0) {
        ERROR("[%s]VIDIOC_REQBUFS failed (%d)\n", UVC_MODULE_NAME, ret);
        return false;
    }

    uvcBuffer = (void *)calloc(width * height * 2 + 1000, sizeof(char));
    if (!uvcBuffer) {
        ERROR("[%s]calloc uvcBuffer failed (%d)\n", UVC_MODULE_NAME, ret);
        return false;
    }

    frameBuf = (UvcFrameBuf *)calloc(reqbuf.count, sizeof(UvcFrameBuf));
    if (!frameBuf) {
        ERROR("[%s]calloc frameBuf failed (%d)\n", UVC_MODULE_NAME, ret);
        free(uvcBuffer);
        uvcBuffer = nullptr;
        return false;
    }
}

bool UVCCapture::SetBuffer()
{
    int ret = 0;
    struct v4l2_buffer buf;
    for (int i = 0; i < UVC_BUFFER_COUNT; i++) {
        ret = memset_s(&buf, sizeof(buf), 0, sizeof(buf));
        if (ret != 0) {
            ERROR("[%s]memset_s buf failed (%d)\n", UVC_MODULE_NAME, ret);
            return false;
        }
        buf.index = i;
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        ret = ioctl(fd, VIDIOC_QUERYBUF, &buf);
        if (ret < 0) {
            ERROR("[%s]VIDIOC_QUERYBUF (%d) failed (%d)\n", UVC_MODULE_NAME, i, ret);
            return false;
        }

        /* mmap buffer */
        frameBuf[i].length = buf.length;
        frameBuf[i].start = (char *)mmap(0, buf.length, PROT_READ | PROT_WRITE, MAP_SHARED, fd, buf.m.offset);
        if (frameBuf[i].start == MAP_FAILED) {
            ERROR("[%s]mmap (%d) failed: %s\n", UVC_MODULE_NAME, i, strerror(errno));
            return false;
        }

        // Queen buffer 将空闲的内存加入到可捕获视频的队列
        ret = ioctl(fd, VIDIOC_QBUF, &buf);
        if (ret < 0) {
            ERROR("[%s]VIDIOC_QBUF (%d) failed (%d)\n", UVC_MODULE_NAME, i, ret);
            return false;
        }
    }
    return true;
}

bool UVCCapture::Init(int dev)
{
    int ret = 0;
    enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    /* 入参检查 , 1、打开视频设备文件, 2、查询视频设备能力 */
    if (!Check(dev)) {
        return false;
    }
    /* 3、设置视频采集参数 */
    if (!SetParam()) {
        return false;
    }
    /* 4、向驱动申请视频流数据的帧缓冲区 */
    if (!MallocBuffer()) {
        return false;
    }
    /* 5、获取每个缓存信息，并mmap到用户空间 */
    if (!SetBuffer()) {
        goto FAIL;
    }
    // 6、开始录制——打开设备视频流
    ret = ioctl(fd, VIDIOC_STREAMON, &type);
    if (ret < 0) {
        ERROR("[%s]VIDIOC_STREAMON failed (%d)\n", UVC_MODULE_NAME, ret);
        goto FAIL;
    }

    return true;

FAIL:
    for (int i = 0; i < UVC_BUFFER_COUNT; i++) {
        if (frameBuf[i].start) {
            munmap(frameBuf[i].start, frameBuf[i].length);
        }
    }
    free(uvcBuffer);
    uvcBuffer = nullptr;
    free(frameBuf);
    frameBuf = nullptr;
    return false;
}

UVCCapture::~UVCCapture()
{
    /* 10、停止视频采集 */
    enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    int ret = ioctl(fd, VIDIOC_STREAMOFF, &type);
    if (ret < 0) {
        ERROR("[%s]VIDIOC_STREAMOFF failed (%d)\n", UVC_MODULE_NAME, ret);
    }

    /* 11、解除映射 */
    for (int i = 0; i < UVC_BUFFER_COUNT; i++) {
        munmap(frameBuf[i].start, frameBuf[i].length);
    }
    free(uvcBuffer);
    uvcBuffer = nullptr;
    free(frameBuf);
    frameBuf = nullptr;

    /* 12、关闭视频设备 */
    close(fd);

    /* 计数清零 */
    readFrameNum = 0;
}

int UVCCapture::Width()
{
    return width;
}

int UVCCapture::Height()
{
    return height;
}

Mat UVCCapture::Read()
{
    int ret = 0;
    Mat dstFrame;
    /* 7、Get frame ——将已经捕获好视频的内存拉出到已捕获视频的队列 */
    struct v4l2_buffer capture_buf;
    ret = memset_s(&capture_buf, sizeof(capture_buf), 0, sizeof(capture_buf));
    if (ret != 0) {
        ERROR("[%s]memset_s capture_buf failed (%d)\n", UVC_MODULE_NAME, ret);
    }
    capture_buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    capture_buf.memory = V4L2_MEMORY_MMAP;
    ret = ioctl(fd, VIDIOC_DQBUF, &capture_buf);
    if (ret < 0) {
        ERROR("[%s]VIDIOC_DQBUF failed (%d)\n", UVC_MODULE_NAME, ret);
    }

    /* 双路放权，每3帧休眠50毫秒 */
    readFrameNum++;
    if (readFrameNum % 3 == 0) {
        usleep(50 * 1000); // 50ms
    }
    /* 8、Data process——数据处理 */
    ret = memset_s(uvcBuffer, width * height * 2 + 1000, 0, frameBuf[capture_buf.index].length);
    if (ret != 0) {
        ERROR("[%s]memset_s uvcBuffer failed (%d)\n", UVC_MODULE_NAME, ret);
    }
    mtx.lock();
    ret = memcpy_s(uvcBuffer, width * height * 2 + 1000, frameBuf[capture_buf.index].start,
        frameBuf[capture_buf.index].length);
    if (ret != 0) {
        ERROR("[%s]memcpy_s uvcBuffer failed (%d)\n", UVC_MODULE_NAME, ret);
    }
    Mat frame = Mat(height, width, CV_8UC3, uvcBuffer);
    mtx.unlock();
    /* 解码成RGB格式 */
    dstFrame = imdecode(frame, CV_LOAD_IMAGE_COLOR);

    /* 9、Re-queen buffer——将刚刚处理完的缓存重新入队列尾，这样可以循环采集 */
    ret = ioctl(fd, VIDIOC_QBUF, &capture_buf);
    if (ret < 0) {
        ERROR("[%s]VIDIOC_QBUF failed (%d)\n", UVC_MODULE_NAME, ret);
    }

    return dstFrame;
}
