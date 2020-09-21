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

#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ei_utils.h"
#include "../include/ei_services.h"


using namespace std;
using namespace hilens;

const string METHODSTR[4] = {"GET", "POST", "PUT", "DELETE"};

EIResponse EIServices::Request(Method method, const string &host, const string &uri, const string &queryParams,
    const string &payload, EIHeaders &headers)
{
    RequestParams request(METHODSTR[method], host, uri, queryParams, payload);
    return EIUtils::GetInstance().SendRequest(request, headers);
}

EIResponse EIServices::HumanDetect(const std::string &imageBase64)
{
    return EIUtils::GetInstance().SendDispatherServiceReq("human-detect", imageBase64);
}

EIResponse EIServices::FaceAttribute(const std::string &imageBase64)
{
    return EIUtils::GetInstance().SendDispatherServiceReq("face-attribute", imageBase64);
}

EIResponse EIServices::LicensePlate(const std::string &imageBase64)
{
    return EIUtils::GetInstance().SendDispatherServiceReq("license-plate", imageBase64);
}

EIResponse EIServices::DogShitDetect(const std::string &imageBase64)
{
    return EIUtils::GetInstance().SendDispatherServiceReq("dogshit-detect", imageBase64);
}

EIResponse EIServices::SearchFace(const std::string &faceSetName, const std::string &imageBase64, int topN,
    double threshold, const std::string &filter)
{
    return EIUtils::GetInstance().SearchFace(faceSetName, imageBase64, topN, threshold, filter);
}

EIResponse EIServices::AddFace(const std::string &faceSetName, const std::string &imageBase64,
    const std::string &externalImageId)
{
    return EIUtils::GetInstance().AddFace(faceSetName, imageBase64, externalImageId);
}

EIResponse EIServices::FaceDetect(const std::string &imageBase64, const std::string &attributes)
{
    return EIUtils::GetInstance().FaceDetect(imageBase64, attributes);
}

EIResponse EIServices::FaceCompare(const std::string &base64Image1, const std::string &base64Image2)
{
    return EIUtils::GetInstance().FaceCompare(base64Image1, base64Image2);
}

EIResponse EIServices::MARealTimeService(const std::string &host, const std::string &url, const std::string &fileKey,
    const std::string &filePath)
{
    return EIUtils::GetInstance().SendRealTimeServiceReq(host, url, fileKey, filePath);
}