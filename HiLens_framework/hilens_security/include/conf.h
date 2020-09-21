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

#ifndef HILENS_SECURITY_CONF_H
#define HILENS_SECURITY_CONF_H

#include <string>
#include <map>
#include "errors.h"
#include "hstring.h"
#include "utils.h"

#define SFW_CONF_PATH "/home/hilens/skillframework/configs/sfw.conf"
#define DEVICE_INFO "/home/hilens/hda/etc/device_info.conf"
#define HDA_CONF_PATH "/home/hilens/hda/etc/hda.conf"
#define OBS_STORAGE_CONF_PATH "/home/hilens/hda/etc/obs_storage_path"

namespace hilens {
class Configuration {
public:
    // hda的配置文件
    static Configuration hdaConfig;
    // 设备信息文件
    static Configuration deviceInfo;
    // SFW的配置文件
    static Configuration sfwConfig;
    // 云侧接口地址
    static Configuration api;
    // 加载hdaConfig和deviceInfo
    static bool Init();

    // 根据key获取value，如果没有会自动插入空字符串到map然后返回
    std::string &operator[](const std::string &key);
    // 检查是否有某配置项
    bool Has(const std::string &key);

protected:
    int Load(const char *filepath);
    std::map<std::string, std::string> data;
    int Parse(const std::string &line);
};

// 设备配置
class DeviceConfig {
public:
    // 加载设备配置
    static HiLensEC Init();
    static bool HasCamera(const std::string &name);
    static Hstring GetCameraURL(const std::string &name);

private:
    struct IPCMeta {
        Hstring username;
        Hstring password;
        Hstring protocol;
        Hstring urn;
    };
    static std::map<std::string, IPCMeta> cameraList;
};

// 上传到obs桶的配置
class OBSConfig {
public:
    static OBSConfig &Instance();
    HiLensEC Init();
    std::string Bucket();
    std::string Domain();
    std::string Prefix();

private:
    OBSConfig() {}
    OBSConfig(const OBSConfig &);
    std::string bucket;
    std::string domain;
    std::string prefix;
};
} // namespace hilens
#endif // HILENS_SECURITY_CONF_H
