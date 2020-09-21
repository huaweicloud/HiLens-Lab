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

#ifndef MEDIA_UTILS_H
#define MEDIA_UTILS_H

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#define OP_TYPE_COUNT 5

typedef unsigned char HI_U8;
typedef unsigned short HI_U16;
typedef unsigned int HI_U32;

typedef signed char HI_S8;
typedef short HI_S16;
typedef int HI_S32;

typedef unsigned long HI_UL;
typedef signed long HI_SL;

typedef float HI_FLOAT;
typedef double HI_DOUBLE;

#ifndef _M_IX86
typedef unsigned long long HI_U64;
typedef long long HI_S64;
#else
typedef unsigned __int64 HI_U64;
typedef __int64 HI_S64;
#endif

typedef HI_U32 VB_POOL;
typedef HI_U32 VB_BLK;

// mpp资源结构体
typedef struct hi_MPP_SRC_INFO {
    int chnID;
    unsigned int vbBlockID;
} MPP_SRC_INFO;

// 媒体操作类型枚举
typedef enum hi_MEDIA_OP_TYPE_E {
    OP_ENCODE = 0, /* 编码 */
    OP_DECODE,     /* 解码 */
    OP_HDMI,       /* 输出到HDMI */
    OP_CAMERA,     /* 摄像头读取数据 */
    OP_PROCESS     /* crop和resize处理 */
} MEDIA_OP_TYPE_E;

// 从VB缓存池映射内存结构体
typedef struct hi_VB_MMAP {
    VB_BLK blockID;
    VB_POOL poolID;
    HI_U8 *pVirYaddr;
    HI_U64 phyYaddr;
    HI_U32 virSize;
} VB_MMAP;

// 媒体资源管理信息
typedef struct hi_MEDIA_SRC_INFO {
    MPP_SRC_INFO mppInfo;

    SAMPLE_VO_CONFIG_S stVoConfig;
    SAMPLE_VI_CONFIG_S stViConfig;

    unsigned int imageWidth;  /* 图片宽度 */
    unsigned int imageHeight; /* 图片高度 */

    unsigned int timeRef; /* 时间戳 */
    VB_MMAP vbMmap;
    char *skillid;
} MEDIA_SRC_INFO;

// 释放内存
#define HI_FREE(p) do {                 \
        if (p != NULL) { \
            free(p);     \
        }                \
        p = NULL;        \
    } while (0)

#define HI_NEW_COPY(src, dest) do {                                           \
        int size = strlen(src);                    \
        dest = (char *)malloc(size + 1);           \
        if (dest == NULL) {                        \
            break;                                 \
        }                                          \
        int ret = memcpy_s(dest, size, src, size); \
        if (ret != 0) {                            \
            free(dest);                            \
            dest = NULL;                           \
            break;                                 \
        }                                          \
        (dest)[size] = '\0';                       \
    } while (0)

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* MEDIA_UTILS_H */
