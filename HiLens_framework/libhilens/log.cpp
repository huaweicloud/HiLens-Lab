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

#include "log.h"
#include "errors.h"
#include <log4cplus/log4cplus.h>
#include <securec.h>

using namespace log4cplus;

namespace hilens {
#define SKILL_LOGGER_NAME "skill_logger"
// 日志pattern，此样式的结果类似于：2019-08-13 14:25:12,996[12345][INFO] Hello
#define LOG_LAYOUT "%D{%Y-%m-%d %H:%M:%S,%q}[%i][%-5p] %m%n"
// 单条日志最大255个字
#define LOG_BUF_SIZE 256

#define DEFINE_LOG_FUNCTION(func, loglevel) void(func)(const char *fmt, ...)                                      \
    {                                                                     \
        Logger logger = Logger::getInstance(SKILL_LOGGER_NAME);           \
        if (!logger.isEnabledFor(loglevel)) {                             \
            return;                                                       \
        }                                                                 \
        char buf[LOG_BUF_SIZE] = {0};                                   \
        int ret = 0;                                                      \
        va_list args;                                                     \
        va_start(args, fmt);                                              \
        ret = vsnprintf_s(buf, sizeof(buf), sizeof(buf) - 1, fmt, args);  \
        if (ret < 0) {                                                    \
            printf("LOG_FUNCTION ERROR! LOG LENGTH NO MORE THAN 255!\n"); \
            return;                                                       \
        }                                                                 \
        va_end(args);                                                     \
        logger.forcedLog(loglevel, buf);                                  \
    }

DEFINE_LOG_FUNCTION(Trace, TRACE_LOG_LEVEL)
DEFINE_LOG_FUNCTION(Debug, DEBUG_LOG_LEVEL)
DEFINE_LOG_FUNCTION(Info, INFO_LOG_LEVEL)
DEFINE_LOG_FUNCTION(Warning, WARN_LOG_LEVEL)
DEFINE_LOG_FUNCTION(Error, ERROR_LOG_LEVEL)
DEFINE_LOG_FUNCTION(Fatal, FATAL_LOG_LEVEL)

void SetLogLevel(LogLevel level)
{
    Logger logger = Logger::getInstance(SKILL_LOGGER_NAME);
    log4cplus::LogLevel ll[] = {
        TRACE_LOG_LEVEL,
        DEBUG_LOG_LEVEL,
        INFO_LOG_LEVEL,
        WARN_LOG_LEVEL,
        ERROR_LOG_LEVEL,
        FATAL_LOG_LEVEL
    };
    logger.setLogLevel(ll[level]);
}

HiLensEC InitSkillLogger()
{
    auto logger = log4cplus::Logger::getInstance(SKILL_LOGGER_NAME);
    logger.setLogLevel(INFO_LOG_LEVEL); // 默认INFO级别

    // 加一个到stdout的appender
    SharedAppenderPtr appender(new ConsoleAppender());
    appender->setLayout(std::unique_ptr<Layout>(new PatternLayout(LOG_LAYOUT)));
    logger.addAppender(appender);
    return OK;
}
} // namespace hilens
