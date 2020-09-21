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

#ifndef HILENS_HSTRING_H
#define HILENS_HSTRING_H

namespace hilens {
class Hstring {
public:
    Hstring();
    virtual ~Hstring();

    // move constructor
    Hstring(Hstring &&tempStr);
    Hstring(const Hstring &hstr);
    Hstring(const char *cstr);
    Hstring(const char *cstr, unsigned long len);

    Hstring &append(const Hstring &hstr);
    Hstring &operator = (const Hstring &right);
    Hstring &operator = (Hstring &&right);

    Hstring operator + (const Hstring &right);
    Hstring &operator += (const Hstring &right);

    unsigned long length() const;
    const char *c_str() const;
    bool empty() const;

protected:
    char *data;
    unsigned long len;
};
} // namespace hilens
#endif
