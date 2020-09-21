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
#ifndef HILENS_SECURITY_WSCLIENT_H
#define HILENS_SECURITY_WSCLIENT_H

#ifndef SF_OFFLINE
#include <string>
#include <thread>
#include <websocketpp/config/asio_no_tls_client.hpp>
#include <websocketpp/client.hpp>

namespace common {
typedef websocketpp::client<websocketpp::config::asio_client> client;
typedef websocketpp::config::asio_client::message_type::ptr message_ptr;
typedef void (*ON_MSG_CALLBACK)(const char *, int);
class WSClient {
public:
    static WSClient &Instance();
    virtual ~WSClient();

    int Init(const char *host, const char *sid);
    /* *
     * @brief 同步连接
     *
     * @return int 0为成功
     */
    int ConnectAsync();
    /* *
     * @brief 异步连接
     *
     * @return int 0为成功
     */
    int Connect();
    void Close();
    void SetMsgCallback(void (*callback)(const char *, int));
    /* *
     * @brief 发送消息，需转发到云测
     *
     * @param msg 消息内容
     * @param length 消息长度
     * @param taskId 任务id
     *
     * @return int 0为成功
     */
    int SendMessage(const char *msg, int length, const char *taskId = NULL);
    /* *
     * @brief 发送心跳消息
     *
     * *@param msg 消息内容
     * @param length 消息长度
     *
     * @return int 0为成功
     */
    int SendHeart(const char *msg, int length, const char *taskId = NULL);

    /* *
     * @brief 发送模型解密消息
     *
     * *@param msg 消息内容
     * @param length 消息长度
     *
     * @return int 0为成功
     */
    int ModelDecrypt(const char *msg, int length, const char *taskId = NULL);

    /* *
     * @brief 发送设备列表消息
     *
     * *@param msg 消息内容
     * @param length 消息长度
     *
     * @return int 0为成功
     */
    int RequestDevicesList(const char *msg, int length, const char *taskId = NULL);

    /* *
     * @brief 发送,
     *
     * @param msg 消息内容
     * @param length 消息长度
     *
     * @return int 0为成功
     */
    int Send(const char *msg, int length);
    bool IsOpen();

private:
    WSClient();
    WSClient(WSClient &);

    void PingThread();
    int Reconnect();

    template <typename T> void ping_on_open(T *c, std::string payload, websocketpp::connection_hdl hdl);
    template <typename T> void on_fail(T *c, websocketpp::lib::error_code ec, websocketpp::connection_hdl hdl);
    template <typename T> void on_close(T *c, websocketpp::lib::error_code ec, websocketpp::connection_hdl hdl);
    void on_message(client *c, websocketpp::connection_hdl hdl, message_ptr msg);

    std::string hostUri;
    client cli;
    websocketpp::connection_hdl hdl;
    std::string skillId;

    ON_MSG_CALLBACK onMsgCallback;

    std::unique_ptr<std::thread> thPing;
    std::unique_ptr<std::thread> thRun;

    bool stop;
    bool open;
    bool connecting;
};
} // namespace common

#endif // SF_OFFLINE
#endif // HILENS_SECURITY_WSCLIENT_H