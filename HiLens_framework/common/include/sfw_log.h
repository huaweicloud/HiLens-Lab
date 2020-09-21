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

#ifndef COMMON_SFW_LOG_H
#define COMMON_SFW_LOG_H

#include <log4cplus/log4cplus.h>
#include "errors.h"

namespace hilens {
// 日志pattern，此样式的结果类似于：2019-08-13 14:25:12,996[12345][INFO][SFW] Hello
#define LOG_LAYOUT "%D{%Y-%m-%d %H:%M:%S,%q}[%i][%-5p][SFW] %m%n"
#define SFW_LOGGER_NAME "sfw_logger"

HiLensEC InitSFWLogger();

#define LOG_DEBUG(...) LOG4CPLUS_DEBUG_FMT(SFW_LOGGER_NAME, __VA_ARGS__)
#define INFO(...) LOG4CPLUS_INFO_FMT(SFW_LOGGER_NAME, __VA_ARGS__)
#define WARN(...) LOG4CPLUS_WARN_FMT(SFW_LOGGER_NAME, __VA_ARGS__)
#define ERROR(...) LOG4CPLUS_ERROR_FMT(SFW_LOGGER_NAME, __VA_ARGS__)
}
#endif