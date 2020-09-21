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

#ifndef HILENS_INCLUDE_OUTPUT_H
#define HILENS_INCLUDE_OUTPUT_H

#include <string>
#include <memory>
#include <json/json.h>
#include "media_process.h"

namespace hilens {
/* *
 * @brief 上传一个文件，此方法会阻塞线程，直至上传结束
 * @param key 上传到obs中的文件名
 * @param filepath 待上传文件的绝对路径
 * @param mode 上传模式，"write" or "append"
 * @return HiLensEC 0为成功，其他为失败
 */
HiLensEC UploadFile(const std::string &key, const std::string &filepath, const std::string &mode);

/* *
 * @brief 异步上传一个文件，会立即返回
 * @param key 待上传文件本地路径
 * @param filepath 待上传文件的绝对路径
 * @param mode 上传模式，"write" or "append"
 * @param callback 回调函数
 * @return HiLensEC 0为成功，其他为失败
 */
HiLensEC UploadFileAsync(const std::string &key, const std::string &filepath, const std::string &mode,
    void (*callback)(int) = NULL);

/* *
 * @brief 上传一个buffer，此方法会阻塞线程，直至上传结束
 * @param key 上传到obs中的文件名
 * @param buffer 待上传buffer的指针
 * @param bufferSize 待上传buffer的大小
 * @param mode 上传模式，"write" or "append"
 * @return HiLensEC 0为成功，其他为失败
 */
HiLensEC UploadBuffer(const std::string &key, const unsigned char *buffer, size_t bufferSize, const std::string &mode);

/* *
 * @brief 异步上传一个buffer，会立即返回
 * @param key 待上传文件本地路径
 * @param buffer 待上传buffer的指针
 * @param bufferSize 待上传buffer的大小
 * @param mode 上传模式，"write" or "append"
 * @param callback 回调函数
 * @return HiLensEC 0为成功，其他为失败
 */
HiLensEC UploadBufferAsync(const std::string &key, std::shared_ptr<const unsigned char> buffer, size_t bufferSize,
    const std::string &mode, void (*callback)(int) = NULL);

/* *
 * @brief 发送一条消息（同步），阻塞直到发送完毕，需要先在Console上配置好订阅。
 * @param subject 邮件主题（仅配置为邮件时有效）长度不能超过170个字符
 * @param message 消息内容 不超过85个字符
 * @return HiLensEC 0为成功，其他为失败
 */
HiLensEC SendMessage(const std::string &subject, const std::string &message);

/* *
 * @brief 发送一条消息（异步），需要先在Console上配置好订阅
 * @param subject 邮件主题（仅配置为邮件时有效）长度不能超过170个字符
 * @param message 消息内容 不超过85个字符
 * @param callback 回调函数
 * @return HiLensEC 0为成功，其他为失败
 */
HiLensEC SendMessageAsync(const std::string &subject, const std::string &message, void (*callback)(int) = NULL);

/* *
 * @brief 发送透传消息。
 * @param msg 消息内容
 * @param length 消息长度，长度不能超过10240
 * @return HiLensEC 0为成功，其他为失败
 */
HiLensEC SendWSSMessage(const char *msg, int length);

/* *
 * @brief 接收透传消息
 *
 * @param callback 接收透传消息回调
 */
void OnWSSMessage(void (*callback)(const char *, int));

/* *
 * @brief 显示器
 * 使用Display类来将图片输出到显示器上
 */
class Display {
public:
    /* *
     * @brief 显示类型
     */
    enum Type {
        HDMI = 0, // 通过HDMI输出
        RTMP,     // 传到指定的RTMP服务器上
        H264_FILE // 输出到文件(h264编码的裸流)
    };

    /* *
     * @brief 构造显示器
     *
     * @param type 显示类型
     * @param path 如果类型为RTMP，则path为RTMP服务器的URL（rtmp://xxxx），
     * 如果为H264_FILE，则path为输出文件的路径（如GetWorkspacePath()+"/out.h264"）
     * @return Display* 成功则返回一个显示器，失败则返回nullptr
     */
    static std::shared_ptr<Display> Create(Type type, const char *path = NULL);

    /* *
     * @brief 显示一张图片
     * 注意，在第一次调用该方法时，Display会根据输入的图片尺寸来设置
     * 视频尺寸，此后的调用中skill必须保证输入图片的尺寸与之前的一致。
     * @param frame 要显示的图片，必须为NV21格式
     * @return 0为成功，其他为失败
     */
    virtual HiLensEC Show(const cv::Mat &frame) = 0;

    virtual ~Display() {}

protected:
    Display() {}
    Display(const Display &);
};

/* *
 * @brief POST请求的头部
 * 调用例如：headers.push_back("Content-Type: application/json");然后将其作为POST的参数传入
 */
typedef std::vector<std::string> POSTHeaders;

/* *
 * @brief 发送一个POST请求。此方法是同步的，会阻塞直到发送完毕。TLS1.2，超时设为20秒
 * @param url 目的统一资源定位符
 * @param body 表示消息体内容的Json对象
 * @param httpcode http请求返回值，如200成功，404不存在
 * @param response 响应，不填则忽略响应
 * @param headers 请求头部，不填则忽略头部
 * @return CURL返回值，0为成功
 */
int POST(const std::string &url, const Json::Value &body, long &httpcode, std::string *response = NULL,
    POSTHeaders *headers = NULL);
} // namespace hilens
#endif // HILENS_INCLUDE_OUTPUT_H