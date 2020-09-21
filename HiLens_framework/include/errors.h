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

#ifndef HILENS_INCLUDE_ERRORS_H
#define HILENS_INCLUDE_ERRORS_H

namespace hilens {
enum HiLensEC {
    // / 没有错误
    OK = 0,
    // / 未知错误
    UNKNOWN_ERROR,
    // / 初始化CURL错误
    INIT_CURL_ERROR,
    // / 创建文件夹失败
    CREATE_DIR_FAILED,
    // / 打开文件失败
    OPENFILE_FAILED,
    // / 重命名失败
    RENAME_FAILED,
    // / 文件不存在或无文件访问权限
    ACCESS_FILE_FAILED,
    // / 无效的BUF
    INVALID_BUF,
    // / 无法解析，查看网络是否通畅
    COULDNT_RESOLVE_HOST,
    // / 写错误，检查下载目录是否有写权限，空间是否足够
    WRITE_ERROR,
    // / 请求超时
    TIMEOUT,
    // / 认证信息错误，检查ak，sk，token是否有效
    AUTH_FAILED,
    // / 没有这个对象
    NOT_FOUND,
    // / 服务端内部错误
    SERVER_ERROR,
    // / 对象冲突
    OBJECT_CONFLICT,
    // / 追加失败（比如追加到不可追加的对象上）
    APPEND_FAILED,
    // / hiai engine发送数据失败，请根据日志来分析具体情况
    HIAI_SEND_DATA_FAILED,
    // / hiai engine推理错误，请根据日志来分析具体情况（可能是实际输入大小与模型的输入大小不匹配）
    HIAI_INFER_ERROR,
    // / 图片处理，src尺寸不符合约束条件
    INVALID_SRC_SIZE,
    // / 图片处理，dst尺寸不符合约束条件
    INVALID_DST_SIZE,
    // / mpp处理图片失败
    MPP_PROCESS_FAILED,
    // / WebSocket错误
    WEBSOCKET_ERROR,
    // / 配置文件错误
    CONFIG_FILE_ERROR,
    // / 参数有误
    INVALID_PARAM,
    // / 内部错误，请检查初始化是否失败,结合日志分析
    INTERNAL_ERROR,
    // / 日志初始化失败
    INIT_LOG_ERROR,
    // /MIC初始化失败
    INIT_MIC_ERROR,
    // / AENC初始化失败
    INIT_AENC_ERROR,
    // / ADEC初始化失败
    INIT_ADEC_ERROR,
    // / AO初始化失败
    INIT_AO_ERROR,
    // / 音频文件不支持
    AUDIO_CHECK_ERROR,
    // / 音频系统初始化失败
    AUDIO_SYSTEM_INIT_FAILED,
    // / 地址非法
    ADDRESS_INVALID,
    // / 连接失败
    CONNECT_FAIL,
    // / SKILL_ID不存在或者非法
    SKILL_ID_INVALID,
    // / 还没连接
    NO_CONNECT,
    // / 发送失败
    SEND_FAIL,
    // / 连接超时
    CONNECT_TIMEOUT,
    // / 消息过长
    MSG_TOO_LONG,
    // / 消息非法
    MSG_INVALID,
    // / 消息队列已满
    MSG_QUEUE_FULL,
};
}
#endif // HILENS_INCLUDE_ERRORS_H