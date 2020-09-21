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

#include "vdecfactory.h"
#include "vdecoder.h"
#include "dvpp_decoder.h"
#include "sf_common.h"
#include "sfw_log.h"

using namespace hilens;

IVDec VDecFactory::Create(unsigned int width, unsigned int height, DecType codec, bool bfast)
{
    return Create(width, height, width, height, codec, bfast);
}

IVDec VDecFactory::Create(unsigned int width, unsigned int height, unsigned int destWidth, unsigned int destHeight,
    DecType codec, bool bfast)
{
    ENCODE_TYPE_E mppcodec = EN_H264;
    if (codec == VDecFactory::DecH265)
        mppcodec = EN_H265;
    VDecoder *pdec = new (std::nothrow) VDecoder(width, height, destWidth, destHeight, mppcodec);
    if (pdec) {
        if (pdec->Init(bfast) != HILENS_OK) {
            ERROR("MppDecoder init failed!");
            return nullptr;
        } else {
            return std::shared_ptr<VDecFactoryInterface>(pdec);
        }
    }

    return nullptr;
}
