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

#ifndef HILENS_INCLUDE_EI_SERVICES_H
#define HILENS_INCLUDE_EI_SERVICES_H

#include <string>
#include <vector>

namespace hilens {
/* *
 * @brief 请求方法，EI_GET,EI_POST,EI_PUT,EI_DELETE分别对应GET,POST,PUT,DELETE
 */
enum Method {
    EI_GET,
    EI_POST,
    EI_PUT,
    EI_DELETE
};
/* *
 * @brief 请求响应
 * @param requestState 请求状态，true表示成功，false表示失败
 * @param responseBody 请求响应
 */
struct EIResponse {
    bool requestState;
    std::string responseBody;
};

class EIServices {
public:
    /* *
     * @brief 请求的头部
     * 调用例如：headers.push_back("Content-Type: application/json");然后将其作为POST的参数传入
     */
    using EIHeaders = std::vector<std::string>;

    /* *
     * @brief 发送请求
     * @param method 请求方法。可选GET,POST,PUT,DELETE
     * @param host 请求域名。host+uri需要是完整的请求url
     * @param uri 请求uri。host+uri需要是完整的请求url
     * @param queryParams 查询字符串。
     * @param payload 请求消息体。
     * @param headers 请求消息头。
     * @return 请求响应
     */
    static EIResponse Request(Method method, const std::string &host, const std::string &uri,
        const std::string &queryParams, const std::string &payload, EIHeaders &headers);

    // Hilens部分接口（人形人脸检测、车牌识别）需要联系hilens工作人员开通相关接口才可使用
    /* *
     * @brief 调用人形人脸检测api
     * @param imageBase64 待识别图像的base64编码。
     * @return 请求响应
     */
    static EIResponse HumanDetect(const std::string &imageBase64);
    /* *
     * @brief 调用人脸属性识别api
     * @param imageBase64 待识别图像的base64编码。
     * @return 请求响应
     */
    static EIResponse FaceAttribute(const std::string &imageBase64);
    /* *
     * @brief 调用车牌识别api
     * @param imageBase64 待识别图像的base64编码。
     * @return 请求响应
     */
    static EIResponse LicensePlate(const std::string &imageBase64);
    /* *
     * @brief 调用狗排泄物识别api
     * @param imageBase64 待识别图像的base64编码。
     * @return 请求响应
     */
    static EIResponse DogShitDetect(const std::string &imageBase64);


    // Face相关接口需要在人脸识别服务开通相关接口才可使用，详细文档请参考人脸识别服务相关文档
    /* *
     * @brief Face服务-人脸搜索
     * @param faceSetName 待搜索的人脸库的名称。
     * @param imageBase64 待识别图像的base64编码。
     * @param topN 返回查询到的最相似的N张人脸。
     * @param threshold 人脸相似度阈值，低于这个阈值则不返回，取值范围0-1。
     * @param filter 过滤条件。
     * @return 请求响应
     */
    static EIResponse SearchFace(const std::string &faceSetName, const std::string &imageBase64, int topN,
        double threshold, const std::string &filter);
    /* *
     * @brief Face服务-添加人脸
     * @param faceSetName 人脸库的名称。
     * @param imageBase64 待添加人脸图像的base64编码。
     * @param externalImageId 用户指定的图片外部ID，与当前图像绑定。用户没提供，系统会生成一个。
     * @return 请求响应
     */
    static EIResponse AddFace(const std::string &faceSetName, const std::string &imageBase64,
        const std::string &externalImageId);

    /* *
     * @brief Face服务-人脸检测，对输入图片进行人脸检测和分析，输出人脸在图像中的位置、人脸关键点位置和人脸关键属性。
     * @param imageBase64 图像数据，Base64编码。
     * @param attributes 是否返回人脸属性，希望获取的属性列表，多个属性间使用逗号（,）隔开。
     * @return 请求响应
     */
    static EIResponse FaceDetect(const std::string &imageBase64, const std::string &attributes);
    /* *
     * @brief Face服务-人脸比对，将两个人脸进行比对，来判断是否为同一个人，返回比对置信度。
     * @param image1Base64 图像数据，Base64编码。
     * @param image2Base64 图像数据，Base64编码。
     * @return 请求响应
     */
    static EIResponse FaceCompare(const std::string &base64Image1, const std::string &base64Image2);

    /* *
     * @brief 访问ModelArts在线服务。
     * @param host 域名
     * @param url 在线服务的请求url。
     * @param fileKey 文件的参数名。
     * @param filePath 文件路径，当前仅支持单张图片预测。
     * @return 请求响应
     */
    static EIResponse MARealTimeService(const std::string &host, const std::string &url, const std::string &fileKey,
        const std::string &filePath);
};
}
#endif // HILENS_INCLUDE_EI_SERVICES_H