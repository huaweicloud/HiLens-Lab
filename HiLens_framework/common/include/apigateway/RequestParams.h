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

#ifndef SRC_REQUESTPARAMS_H_
#define SRC_REQUESTPARAMS_H_
#include <string>
#include <set>
#include "header.h"

class RequestParams {
public:
    RequestParams();
    RequestParams(std::string method, std::string host, std::string uri, std::string queryParams);
    RequestParams(std::string method, std::string host, std::string uri, std::string queryParams, std::string payload);
    virtual ~RequestParams();

    const std::string getMethod();
    const std::string getHost();
    const std::string getUri();
    const std::string getQueryParams();
    const std::string getPayload();
    const std::set<Header> *getHeaders();

    void addHeader(Header &header);
    void addHeader(std::string key, std::string value);
    std::string initHeaders();

private:
    /* HTTP Request Parameters */
    std::string mMethod;
    std::string mHost;
    std::string mUri;
    std::string mQueryParams;
    std::string mPayload;

    std::set<Header> mHeaders;
};

#endif /* SRC_REQUESTPARAMS_H_ */
