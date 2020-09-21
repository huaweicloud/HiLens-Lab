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

#ifndef COMMON_MESSAGE_MGR_H
#define COMMON_MESSAGE_MGR_H

#ifndef SF_OFFLINE
#include <chrono>
#include <functional>
#include <map>
#include <mutex>
#include <memory>
#include <string>
#include "wsclient.h"
#include "../libhilens/BlockingQueue.h"

namespace common {
class TimeoutMsg {
public:
    TimeoutMsg()
    {
        createTime = Time::now();
    }
    virtual ~TimeoutMsg() {}
    static const int SECOND_TIME_MS = 1000;
    static const int TIME_OUT = 10;
    using Time = std::chrono::high_resolution_clock;
    using ms = std::chrono::duration<double, std::ratio<1, SECOND_TIME_MS>>;
    using elaspms = std::chrono::duration<float>;
    virtual bool IsMessageTimeOut()
    {
        elaspms fs = Time::now() - createTime;
        double timeUsed = std::chrono::duration_cast<ms>(fs).count();
        return (int)(timeUsed / SECOND_TIME_MS) >= TIME_OUT;
    }

protected:
    std::chrono::system_clock::time_point createTime;
};

class MessageInfo : public TimeoutMsg {
public:
    MessageInfo(std::function<void(const char *msg, int)> cb)
    {
        callback = cb;
    }
    virtual ~MessageInfo() {}
    std::function<void(const char *msg, int)> callback;
};
class MessageMgr {
public:
    static MessageMgr &Instance();
    virtual ~MessageMgr();

    /* *
     * @brief 发送消息，需转发到云测
     *
     * @param msg 消息内容
     * @param length 消息长度
     * @param callback 回调
     *
     * @return int 0为成功
     */
    int Send(const char *msg, int length, std::function<void(const char *msg, int)> callback = NULL);

    void OnMessage(const char *msg, int length, const std::string &uuid);

private:
    MessageMgr();
    MessageMgr(MessageMgr &);
    std::string Getuuid();
    void ClearOldMsg();

private:
    std::map<std::string, std::unique_ptr<MessageInfo>> msgMap;
    std::mutex mapMutex;
};
}
#endif // SF_OFFLINE
#endif // COMMON_MESSAGE_MGR_H