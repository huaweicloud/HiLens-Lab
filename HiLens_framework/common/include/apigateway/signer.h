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

#ifndef SRC_SIGNER_H_
#define SRC_SIGNER_H_

#include <iostream>
#include <string>
#include <vector>
#include <set>
#include <map>
#include <ctime>
#include "header.h"
#include "hasher.h"
#include "RequestParams.h"

class Signer {
public:
    Signer();
    Signer(std::string appKey, std::string appSecret);
    ~Signer();

    /* Task 1: Get Canonicalized Request String */
    const std::string getCanonicalRequest(std::string signedHeaders, std::string method, std::string uri,
        std::string query, const std::set<Header> *headers, std::string payload);

    const std::string getCanonicalURI(std::string &uri);
    const std::string getCanonicalQueryString(std::map<std::string, std::string> &queryParams);
    const std::string getCanonicalQueryString(std::map<std::string, std::vector<std::string>> &queryParams);
    const std::string getCanonicalQueryString(std::string &queryParams);
    const std::string getCanonicalHeaders(const std::set<Header> *headers);
    const std::string getSignedHeaders(const std::set<Header> *headers);
    const std::string getHexHash(std::string &payload);

    /* Task 2: Get String to Sign */
    const std::string getStringToSign(std::string algorithm, std::string date, std::string canonicalRequest);

    /* Task 3: Calculate the Signature */
    const std::string getSignature(const char *signingKey, const std::string &stringToSign);


    // One stroke create signature
    const std::string createSignature(RequestParams *request);

private:
    /* Credentials */
    std::string mAppKey;
    std::string mAppSecret;
    Hasher *hasher;
};

#endif /* SRC_SIGNER_H_ */
