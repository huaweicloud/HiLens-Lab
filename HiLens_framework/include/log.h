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

#ifndef HILENS_INCLUDE_LOG_H
#define HILENS_INCLUDE_LOG_H

#include <string>

namespace hilens {
/* *
 * @brief 日志级别
 */
enum LogLevel {
    TRACE,
    DEBUG,
    INFO,
    WARNING,
    ERROR,
    FATAL
};

/* *
 * @brief 打印一条TRACE级别的日志
 * @details 可按照printf的风格打印日志
 * @param fmt 日志格式
 * @param ... 日志的参数
 */
void Trace(const char *fmt, ...);

/* *
 * @brief 打印一条DEBUG级别的日志
 * @details 可按照printf的风格打印日志
 * @param fmt 日志格式
 * @param ... 日志的参数
 */
void Debug(const char *fmt, ...);

/* *
 * @brief 打印一条INFO级别的日志
 * @details 可按照printf的风格打印日志
 * @param fmt 日志格式
 * @param ... 日志的参数
 */
void Info(const char *fmt, ...);

/* *
 * @brief 打印一条WARNING级别的日志
 * @details 可按照printf的风格打印日志
 * @param fmt 日志格式
 * @param ... 日志的参数
 */
void Warning(const char *fmt, ...);

/* *
 * @brief 打印一条ERROR级别的日志
 * @details 可按照printf的风格打印日志
 * @param fmt 日志格式
 * @param ... 日志的参数
 */
void Error(const char *fmt, ...);

/* *
 * @brief 打印一条FATAL级别的日志
 * @details 可按照printf的风格打印日志
 * @param fmt 日志格式
 * @param ... 日志的参数
 */
void Fatal(const char *fmt, ...);

/* *
 * @brief 设置日志级别
 * @param level 日志级别
 */
void SetLogLevel(LogLevel level);
} // namespace hilens
#endif // HILENS_INCLUDE_LOG_H