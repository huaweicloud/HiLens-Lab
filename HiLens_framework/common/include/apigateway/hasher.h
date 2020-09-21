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

#ifndef SRC_HASHER_H_
#define SRC_HASHER_H_

#include <string>
#include "constants.h"

class Hasher {
public:
    Hasher();
    ~Hasher();
    const std::string hexEncode(unsigned char *md, size_t len);
    int hashSHA256(const std::string &str, int len, unsigned char *hash);
    unsigned char *hmac(const void *key, unsigned int keyLength, const std::string &data);
};

#endif /* SRC_HASHER_H_ */
