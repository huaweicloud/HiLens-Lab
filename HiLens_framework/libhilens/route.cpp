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

#ifndef SF_OFFLINE
#include "route.h"
#include "sfw_log.h"
#include "utils.h"
#include "auth.h"

using namespace std;

namespace hilens {
Router &Router::Instance()
{
    static Router route;
    return route;
}

Router::Router()
{
    // 指定各个消息对应的处理函数
    // token查询
    funcs["token_query"] = [](const Json::Value &doc) { Auth::Instance().OnTokenQuery(doc); };
}

Router::~Router() {}

void Router::Route(const string &msg)
{
    // 解析并初步检查错误
    Json::Value doc;
    string errorMsg;
    if (!String2Json(msg, doc, errorMsg)) {
        WARN("Received invalid message. %s", errorMsg.c_str());
        return;
    }
    if (!doc.isMember("topic") || !doc["topic"].isString()) {
        WARN("Received invalid message. 'topic' string not found.");
        return;
    }

    // 开始分类
    string topic = doc["topic"].asString();

    if (funcs.find(topic) == funcs.end()) {
        WARN("Received invalid message. Unknown 'topic'.");
        return;
    }

    // 调用相关函数
    funcs[topic](doc);
}
}
#endif
