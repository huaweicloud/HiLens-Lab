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

#ifndef SF_COMMIN_H
#define SF_COMMIN_H

// C释放内存
#define SF_FREE(p) do {                 \
        if (p != NULL) { \
            free(p);     \
        }                \
        p = NULL;        \
    } while (0)

// C++释放内存
#define SF_DELETE(p) do {                    \
        if (p != nullptr) { \
            delete p;       \
        }                   \
        p = nullptr;        \
    } while (0)

#define HILENS_OK 0
#define HILENS_ERROR 1

namespace hilens {
class SFCommon {
public:
    static int GetFileSize(const char *fileName);

    static int ReadFile(const char *filePath, unsigned char **fileBuff, int *buffSize);

    static int WriteFile(const char *filePath, const unsigned char *fileBuff, int buffSize);

private:
    SFCommon();
    ~SFCommon();
};
} // namespace hilens

#endif