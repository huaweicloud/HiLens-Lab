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

#include "resource.h"
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <string.h>
#include <iomanip>
#include <openssl/md5.h>
#include "utils.h"
#include "sfw_log.h"
#include "obszilla/obszilla.h"

namespace hilens {
using namespace std;
using namespace hilens;

// 计算哈希时读取文件的batch大小
#define BATCHLEN 65536

const char *GetCWD()
{
    static char cwd[PATH_MAX] = {0};
    static bool get = false;
    if (!get) {
        getcwd(cwd, PATH_MAX);
        size_t len = strlen(cwd);
        cwd[len] = '/';
        cwd[len + 1] = '\0';
        get = true;
    }
    return cwd;
}

// 返回技能可读写目录的路径
// 如果不允许在技能安装目录下写操作，则需要制定各技能可写的位置
string GetWorkspacePath()
{
    char *workspace = getenv("SKILL_WORKSPACE");
    if (workspace) {
        return workspace;
    } else {
        ERROR("Failed to get SKILL_WORKSPACE!");
        return GetCWD();
    }
}

string GetModelDirPath()
{
#ifdef SF_OFFLINE
    ERROR("Not supported in OFFLINE version.");
    return GetCWD();
#endif
    char *modeldir = getenv("SKILL_MODEL_PATH");
    if (modeldir) {
        return modeldir;
    } else {
        ERROR("Failed to get SKILL_MODEL_PATH!");
        return GetCWD();
    }
}

Json::Value GetSkillConfig()
{
#ifdef SF_OFFLINE
    ERROR("Not supported in OFFLINE version.");
    return Json::Value();
#endif
    string errorMsg;
    Json::Value conf;

    if (!File2Json(GetWorkspacePath().append("skill_config.json"), conf, errorMsg)) {
        ERROR("Failed to load skill config. %s", errorMsg.c_str());
        return Json::Value();
    }

    return conf;
}

std::string MD5ofFile(const std::string &filepath)
{
    unsigned char md5[MD5_DIGEST_LENGTH];
    char *batchdata = new char[BATCHLEN];

    char resolvedPath[PATH_MAX];
    if (NULL == realpath(filepath.c_str(), resolvedPath)) {
        ERROR("Failed to resolve root path.");
        delete[] batchdata;
        return "";
    }

    FILE *fp = fopen(resolvedPath, "rb");
    if (!fp) {
        ERROR("Failed to open file.");
        delete[] batchdata;
        return "";
    }

    int batch = fread(batchdata, 1, BATCHLEN, fp);
    MD5((unsigned char *)batchdata, batch, md5);
    fclose(fp);

    stringstream ss;
    for (int i = 0; i < MD5_DIGEST_LENGTH; ++i) {
        ss << hex << setw(2) << setfill('0') << (int)md5[i];
    }
    delete[] batchdata;
    return ss.str();
}

HiLensEC DownloadFileFromOBS(const std::string &url, const std::string &downloadTo)
{
#ifdef SF_OFFLINE
    ERROR("Not supported in OFFLINE version.");
    return UNKNOWN_ERROR;
#endif
    OBSZilla obs;
    obs.downloadTo = downloadTo;
    return obs.Download(url);
}
}