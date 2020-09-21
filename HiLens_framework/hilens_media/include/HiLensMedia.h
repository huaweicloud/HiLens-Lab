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

#ifndef HILENS_MEDIA_H
#define HILENS_MEDIA_H

#include "MediaComm.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

typedef enum hiENCODE_TYPE_E {
    EN_H265 = 0,
    EN_H264 = 1
} ENCODE_TYPE_E;

typedef struct hi_SHOW_HDMI_PARAM {
    unsigned int imageWidth;       /* 图片宽度 */
    unsigned int imageHeight;      /* 图片高度 */
    HILENS_VO_OUTPUT_E outputSize; /* 显示尺寸和分辨率类型 */
    char *skillid;                 /* 要使用HDMI输出的技能id */
} SHOW_HDMI_PARAM;

typedef struct hi_ENCODE_PARAM {
    ENCODE_TYPE_E encodeType; /* 编码类型：目前支持H264和H265编码 */

    unsigned int imageWidth;  /* 图片宽度 */
    unsigned int imageHeight; /* 图片高度 */
    unsigned int frameRate;   /* 编码帧率 */
    HILENS_RC_E rcMode;
    HILENS_VENC_GOP_MODE_E gopMode;

    char *skillid; /* 要使用编码的技能id */
} ENCODE_PARAM;

typedef struct hi_DECODE_PARAM {
    ENCODE_TYPE_E encodeType; /* 编码类型：目前支持H264编码的视频解码 */

    unsigned int imageWidth;  /* 原始图片宽度 */
    unsigned int imageHeight; /* 原始图片高度 */
    unsigned int frameRate;   /* 编码帧率 */

    unsigned int destWidth;           /* 解码后图片宽度 */
    unsigned int destHeight;          /* 解码后图片高度 */
    H_PIXEL_FORMAT_E destFixelFormat; /* 解码图片格式 */

    char *skillid; /* 要使用解码的技能id */
} DECODE_PARAM;

typedef struct hi_CAMERA_PARAM {
    H_PIXEL_FORMAT_E fixelFormat; /* 获取的摄像头图片格式 */
    unsigned int imageWidth;      /* 图片宽度，范围[64, 1920], 2的倍数 */
    unsigned int imageHeight;     /* 图片高度，范围[64, 1080], 2的倍数 */
    char *skillid;                /* 要使用camera输出的技能id */
} CAMERA_PARAM;

typedef struct hi_PROCESS_PARAM {
    H_PIXEL_FORMAT_E fixelFormat; /* 处理后的图片格式，支持NV21和NV12 */

    unsigned char *imageBuf; /* 原始的图片缓存，只支持NV21 */
    unsigned int srcWidth;   /* 原始的图片宽度，范围[64, 1920], 2的倍数 */
    unsigned int srcHeight;  /* 原始的图片高度，范围[64, 1080], 2的倍数 */

    unsigned int startX; /* 裁剪的起始X坐标，范围[0, 1920], 2的倍数。如果resize操作，该参数被忽略 */
    unsigned int startY; /* 裁剪的起始Y坐标，范围[0, 1080], 2的倍数。如果resize操作，该参数被忽略 */

    unsigned int destWidth;  /* 处理后的图片宽度，范围[64, 1920], 2的倍数 */
    unsigned int destHeight; /* 处理后的图片高度，范围[64, 1080], 2的倍数 */
} PROCESS_PARAM;

/* ----------------------------------------------*
 * 全局初始化
 * ---------------------------------------------- */

/* *
 * 创建使用媒体库的系统环境接口。
 * 该接口用于创建媒体库使用的系统环境，推荐在守护进程中调用，全局环境只创建一次。
 * 该进程会阻塞线程进行监听其他进程的请求，进行全局资源分配，如果主线程不能阻塞，请在子线程调用该接口。
 * @return int          返回错误码, 成功返回HILENS_SUCCESS，失败返回HILENS_FAILURE
 */
int CreateSystemEnv();

/* *
 * 销毁使用媒体库的系统环境接口。
 * 该接口用于销毁媒体库使用的系统环境，推荐在守护进程中调用，只在媒体库使用完调用一次。
 */
void DestroySystemEnv();

/* ----------------------------------------------*
 * 显示图片到HDMI
 * ---------------------------------------------- */

/* *
 * 创建图片显示到本地HDMI环境接口。
 * 该接口用于配置显示到HDMI所需环境。推荐使用HILENS_VO_OUTPUT_1080P60显示类型。
 * 目前只支持一路数据输出到HDMI，多路输出会导致画面异常
 * @param params        输入参数，显示配置参数
 * @return int          返回错误码, 成功返回HILENS_SUCCESS，失败返回HILENS_FAILURE
 */
int CreateShowEnv(SHOW_HDMI_PARAM params);

/*
 * 显示图片到HDMI接口。
 * 该接口将输入的图片输出到本地HDMI进行显示，只支持YVU420_NV21类型图片。
 * @param imageBuf      输入参数，待显示到HDMI的图片缓存
 * @param bufSize       输入参数，待显示到HDMI的图片缓存大小
 * @return int          返回错误码, 成功返回HILENS_SUCCESS，失败返回HILENS_FAILURE
 */
int ShowToLocalHDMI(const unsigned char *imageBuf, unsigned int bufSize);

/*
 * 销毁显示到本地HDMI环境接口。
 * 该接口负责显示结束后销毁环境资源。
 * @return int          返回错误码, 成功返回HILENS_SUCCESS，失败返回HILENS_FAILURE
 */
int DestroyShowEnv();

/* ----------------------------------------------*
 * 解码
 * ---------------------------------------------- */

/* *
 * 创建解码环境接口。
 * 该接口用于配置解码所需参数，创建解码所需环境。
 * @param params        输入参数，解码配置参数
 * @return int          返回错误码, 成功返回HILENS_SUCCESS，失败返回HILENS_FAILURE
 */
MediaHandle CreateDecodeEnv(DECODE_PARAM params);

/*
 * 解码接口。
 * 该接口对输入的编码帧进行解码处理，支持多通道多线程同时解码。
 * @param chnID         输入参数，解码通道ID, 取值范围[0, chnNum)
 * @param decodeBuf     输入参数，待解码的图片缓存
 * @param bufSize       输入参数，待解码的图片缓存大小
 * @param outParams     输出参数，解码后的图片数据，outData由接口负责申请内存，调用者使用完后释放
 * @return int          返回错误码, 成功返回HILENS_SUCCESS，失败返回HILENS_FAILURE
 */
int Decode(MediaHandle handle, const unsigned char *decodeBuf, int bufSize, OUTPUT_PARAM *outParams);

/*
 * 销毁解码环境接口。
 * 该接口负责解码结束后销毁解码环境资源。
 * @return int          返回错误码, 成功返回HILENS_SUCCESS，失败返回HILENS_FAILURE
 */
int DestroyDecodeEnv(MediaHandle handle);

/* ----------------------------------------------*
 * 编码
 * ---------------------------------------------- */

/* *
 * 创建编码环境接口。
 * 该接口用于配置编码所需参数，创建编码所需环境。推荐使用720P，1080P编码。
 * @param params        输入参数，编码配置参数
 * @return int          返回错误码, 成功返回HILENS_SUCCESS，失败返回HILENS_FAILURE
 */
MediaHandle CreateEncodeEnv(ENCODE_PARAM params);

/*
 * 编码接口。
 * 该接口对输入的图片缓存进行编码处理，支持多通道多线程同时编码。
 * @param chnID         输入参数，编码通道ID, 取值范围[0, chnNum)
 * @param encodeBuf     输入参数，待编码的图片缓存
 * @param bufSize       输入参数，待编码的图片缓存大小
 * @param outParams     输出参数，编码后的视频流数据，outData由接口负责申请内存，调用者使用完后释放
 * @return int          返回错误码, 成功返回HILENS_SUCCESS，失败返回HILENS_FAILURE
 */
int Encode(MediaHandle handle, const unsigned char *encodeBuf, unsigned int bufSize, OUTPUT_PARAM *outParams);

/*
 * 销毁编码环境接口。
 * 该接口负责编码结束后销毁编码环境资源。
 * @return int          返回错误码, 成功返回HILENS_SUCCESS，失败返回HILENS_FAILURE
 */
int DestroyEncodeEnv(MediaHandle handle);

/* ----------------------------------------------*
 * 从摄像头读取数据
 * ---------------------------------------------- */
/* *
 * 创建本地摄像头环境接口。
 * 该接口用于配置本地摄像头启动所需参数，创建摄像头所需环境。
 * @param params        输入参数，摄像头配置参数
 * @return int          返回错误码, 成功返回HILENS_SUCCESS，失败返回HILENS_FAILURE
 */
int CreateCameraEnv(CAMERA_PARAM params);

/*
 * 读取摄像头图片数据接口。
 * @brief 该接口从摄像头读取数据，目前只支持单个进程读取。
 * @param outFrame     输出参数，从摄像头读取的原始帧数据，outFrame由接口负责申请内存，调用者使用完后释放
 * @param size         输入输出参数，从摄像头读取原始帧的图片缓存大小
 */
int ReadFrameFromCamera(OUTPUT_PARAM *outFrame);

/*
 * 销毁本地摄像头环境接口。
 * 该接口负责使用本地摄像头结束后销毁摄像头环境资源。
 * @return int          返回错误码, 成功返回HILENS_SUCCESS，失败返回HILENS_FAILURE
 */
int DestroyCameraEnv();

/* ----------------------------------------------*
 * 图片预处理接口
 * ---------------------------------------------- */
/* *
 * 创建图片处理环境接口。
 * 该接口用于配置图片resize和crop的环境接口。
 * @param skillid        输入参数，使用图片resize和crop的技能id
 * @param chnNum         输入参数，单个进程并发使用处理接口的数量
 * @return MediaHandle   成功返回媒体环境句柄，失败返回NULL
 */
MediaHandle CreateProcessEnv(const char *skillid);

/*
 * 销毁图片处理环境接口。
 * 该接口负责图片处理后销毁环境资源。
 * @param MediaHandle   输入参数，CreateProcessEnv获取的媒体环境句柄
 * @return int          返回错误码, 成功返回HILENS_SUCCESS，失败返回HILENS_FAILURE
 */
int DestroyProcessEnv(MediaHandle handle);

/*
 * 缩放图片接口。
 * 该接口对图片进行缩放操作。
 * @param MediaHandle    输入参数，CreateProcessEnv获取的媒体环境句柄
 * @param params         输入输出参数，配置缩放信息
 * @param outParams      输出参数，缩放后的图片缓存，需要调用者使用完之后释放内存
 */
int ResizeImage(MediaHandle handle, const PROCESS_PARAM *params, OUTPUT_PARAM *outParams);

/*
 * 裁剪图片接口。
 * 该接口对图片进行裁剪操作。
 * @param MediaHandle    输入参数，CreateProcessEnv获取的媒体环境句柄
 * @param params         输入输出参数，配置裁剪信息
 * @param outParams      输出参数，裁剪后的图片缓存，需要调用者使用完之后释放内存
 */
int CropImage(MediaHandle handle, const PROCESS_PARAM *params, OUTPUT_PARAM *outParams);

/* *
 * @brief: 获取当前解码最大宽高
 * @return: 返回最大值
 * @date: 2020-04-21
 */
int getMaxWidth();
int getMaxHeight();
/* *
 * @brief: 设置当前解码最大宽高
 * @date: 2020-04-21
 */
void setMaxWh(int w, int h);
/* *
 * @brief: 切换解码宽高，4k的解码buffer会比较小，如果始终是4k的， 1k的解码就会导致buffer不够
 * @return: 是否进行切换  HILENS_NEED4KSWITCH， 切换， HILENS_SUCCESS 宽高满足不用切换
 * @date: 2020-04-21
 */
int switchMppBuffer(int w, int h);
/* *
 * @brief: 异步解码接口，一个线程不停的sendbuf，一个线程不停的获取buf
 * @date: 2020-04-21
 */
void MppSendDecEnd(MediaHandle handle);
int MppSendDecBuf(MediaHandle handle, const unsigned char *decodeBuf, int bufSize);
int MppHasDecBuf(MediaHandle handle, void *pframe_info, int timeout);
int MppGetDecBuf(MediaHandle handle, OUTPUT_PARAM *outParams, void *pframe_info);
/* 高速刷新接口，不进行内存拷贝 */

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* HILENS_MEDIA_H */
