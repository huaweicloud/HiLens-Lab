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

#ifndef MEDIA_COMM_H
#define MEDIA_COMM_H

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

typedef enum {
    H_FALSE = 0,
    H_TRUE = 1,
} H_BOOL;

// 支持图片格式
typedef enum hi_PIXEL_FORMAT_E {
    YVU_SEMIPLANAR_420 = 0, /* NV21，YVU 420, 推荐 */
    YUV_SEMIPLANAR_420,     /* NV12，YUV 420 */
} H_PIXEL_FORMAT_E;

// 图片编码RC类型
typedef enum hiLens_RC_E {
    HILENS_RC_CBR = 0,
    HILENS_RC_VBR,
    HILENS_RC_AVBR,
    HILENS_RC_QPMAP,
    HILENS_RC_FIXQP
} HILENS_RC_E;

// 图片编码GOP模式类型
typedef enum hiLens_VENC_GOP_MODE_E {
    HILENS_VENC_GOPMODE_NORMALP = 0,   /* NORMALP */
    HILENS_VENC_GOPMODE_DUALP = 1,     /* DUALP;  Not support for Hi3556AV100 */
    HILENS_VENC_GOPMODE_SMARTP = 2,    /* SMARTP; Not support for Hi3556AV100 */
    HILENS_VENC_GOPMODE_ADVSMARTP = 3, /* ADVSMARTP ; Only used for Hi3559AV100 */
    HILENS_VENC_GOPMODE_BIPREDB = 4,   /* BIPREDB ;Only used for Hi3559AV100/Hi3519AV100 */
    HILENS_VENC_GOPMODE_LOWDELAYB = 5, /* LOWDELAYB; Not support */
    HILENS_VENC_GOPMODE_BUTT
} HILENS_VENC_GOP_MODE_E;

// 图片输出到HDMI分辨率
typedef enum hiLens_VO_OUTPUT_E {
    HILENS_VO_OUTPUT_1080P60 = 0,  /* 1920 x 1080 at 60 Hz. */
    HILENS_VO_OUTPUT_1080P50,      /* 1920 x 1080 at 50 Hz. */
    HILENS_VO_OUTPUT_1080P30,      /* 1920 x 1080 at 30 Hz. */
    HILENS_VO_OUTPUT_720P50,       /* 1280 x  720 at 50 Hz. */
    HILENS_VO_OUTPUT_720P60,       /* 1280 x  720 at 60 Hz. */
    HILENS_VO_OUTPUT_3840x2160_30, /* 3840x2160_30 */
    HILENS_VO_OUTPUT_3840x2160_50, /* 3840x2160_50 */
    HILENS_VO_OUTPUT_3840x2160_60  /* 3840x2160_60 */
} HILENS_VO_OUTPUT_E;

// 接口输出参数结构体
typedef struct hi_OUTPUT_PARAM {
    void *outData;
    unsigned int size;
    unsigned int destwidth;
    unsigned int allignwidth;
    unsigned int height;
} OUTPUT_PARAM;

// 媒体资源Handle
typedef void *MediaHandle;

#define HILENS_SUCCESS 0
#define HILENS_FAILURE (-1)
#define HILENS_RESOURCE_NOT_ENOUGH (-2) // 资源不足
#define HILENS_NEED4KSWITCH 1

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* MEDIA_COMM_H */
