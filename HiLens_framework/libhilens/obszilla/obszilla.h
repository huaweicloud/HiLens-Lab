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

#ifndef OBSZILLA_OBSZILLA_H
#define OBSZILLA_OBSZILLA_H

#include <string>
#include <map>
#include "errors.h"
#include "request.h"

namespace hilens {
// 描述一个要下载/上传的OBS对象
struct OBSObject {
    std::string bucket; // 对象所在的桶
    std::string domain; // 对象所在的域
    std::string object; // 对象资源路径
};

class OBSZilla {
public:
    // 上传模式
    enum UploadMode {
        WRITE, // 写模式
        APPEND // 追加模式
    };

    // 下载到哪个目录下
    std::string downloadTo;

    // 传输超时设置，当传输时间超过timeout时放弃治疗（毕竟万一网速太差，下几天也不是个办法）
    // 默认为0（永不超时，注意是已经建立连接后传输时的超时设置，内部设置了建立连接的超时限制为30s）
    unsigned int timeout;
    // 是否打印详情到stdout，1为打印，0为不打印。默认为0
    int verbose;

    // 全局初始化，在实例化OBSZilla并调用任何方法之前应先调用此静态方法
    static HiLensEC GlobalInit();

    // 构造函数
    OBSZilla();

    // 析构函数
    virtual ~OBSZilla();

    // 下载一个OBS对象(链接的形式，如https://bucket.obs.cn-north-4.myhuaweicloud.com/test.txt)
    // 除了RequestURL类型，url可以为std::string
    HiLensEC Download(const RequestURL &url);

    // 下载一个OBS对象(链接的形式)
    HiLensEC Download(const char *url);

    // 下载一个OBS对象(根据桶名、域名和对象路径)
    HiLensEC Download(const OBSObject &obj);

    // 上传一个文件，将filepath指向的文件存到OBS，OBS信息填在obj里
    // filepath: 本地文件路径
    // obj: OBS对象信息
    // mode: 上传方式
    HiLensEC Upload(const std::string &filepath, const OBSObject &obj, const UploadMode mode = WRITE);

    // 上传一个buffer，buffer指针为buf，buffer字节数为bufSize，OBS信息填在obj里
    // buf: buffer指针
    // bufSize: buffer字节数
    // obj: OBS对象信息
    // mode: 上传方式
    HiLensEC UploadBuffer(const char *buf, size_t bufSize, const OBSObject &obj, const UploadMode mode = WRITE);

protected:
    // 鉴权信息
    OBSAuth GetAuth();
    static std::map<int, HiLensEC> reqError;
    static std::map<int, HiLensEC> httpError;
    HiLensEC GetErrorCode(int ret, int httpcode);
    // 获取云上对象下次追加的位置
    // 返回对象追加的位置，如果不存在则返回"0"，如果请求错误返回空字符串
    std::string GetObjectNextAppendPos(const OBSObject &obj);
    int Mkdirs(const std::string &FileDir);
};
} // namespace hilens
#endif /* OBSZILLA_OBSZILLA_H */