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

#ifndef SRC_HEADER_H_
#define SRC_HEADER_H_

#include <string>
#include <vector>
#include "utils.h"

class Header {
public:
    Header(const std::string key)
    {
        mKey = key;
    }

    Header(const std::string &key, const std::string &value)
    {
        mKey = key;
        mValues = value;
    }

    std::string getKey()
    {
        return mKey;
    }

    const std::string &getValue()
    {
        return mValues;
    }

    void setValue(std::string value)
    {
        mValues = value;
    }

    bool operator < (const Header &r) const
    {
        if (toLowerCaseStr(this->mKey).compare(toLowerCaseStr(r.mKey)) < 0) {
            return true;
        } else {
            return false;
        }
    }
    ~Header() {}

private:
    std::string mKey;
    std::string mValues;
};


#endif /* SRC_HEADER_H_ */
