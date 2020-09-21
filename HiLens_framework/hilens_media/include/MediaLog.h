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

#ifndef MEDIA_LOG_H
#define MEDIA_LOG_H

#include "zlog.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

extern zlog_category_t *g_cat;

#define LOG_FATAL(...)                                                                                          \
    if (g_cat) {                                                                                                \
        zlog(g_cat, __FILE__, sizeof(__FILE__) - 1, __func__, sizeof(__func__) - 1, __LINE__, ZLOG_LEVEL_FATAL, \
            __VA_ARGS__);                                                                                       \
    }

#define LOG_ERROR(...)                                                                                          \
    if (g_cat) {                                                                                                \
        zlog(g_cat, __FILE__, sizeof(__FILE__) - 1, __func__, sizeof(__func__) - 1, __LINE__, ZLOG_LEVEL_ERROR, \
            __VA_ARGS__);                                                                                       \
    }

#define LOG_WARN(...)                                                                                          \
    if (g_cat) {                                                                                               \
        zlog(g_cat, __FILE__, sizeof(__FILE__) - 1, __func__, sizeof(__func__) - 1, __LINE__, ZLOG_LEVEL_WARN, \
            __VA_ARGS__);                                                                                      \
    }

#define LOG_INFO(...)                                                                                          \
    if (g_cat) {                                                                                               \
        zlog(g_cat, __FILE__, sizeof(__FILE__) - 1, __func__, sizeof(__func__) - 1, __LINE__, ZLOG_LEVEL_INFO, \
            __VA_ARGS__);                                                                                      \
    }

#define LOG_DEBUG(...)                                                                                          \
    if (g_cat) {                                                                                                \
        zlog(g_cat, __FILE__, sizeof(__FILE__) - 1, __func__, sizeof(__func__) - 1, __LINE__, ZLOG_LEVEL_DEBUG, \
            __VA_ARGS__);                                                                                       \
    }

#define CHECK_RETURN(express, name) do {                                                \
        HI_S32 Ret;                                     \
        Ret = express;                                  \
        if (HI_SUCCESS != Ret) {                        \
            LOG_INFO("%s failed with %#x!", name, Ret); \
            return Ret;                                 \
        }                                               \
    } while (0)

int InitLog();

void FinalizeLog();

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* MEDIA_LOG_H */