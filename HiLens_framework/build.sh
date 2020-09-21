#!/bin/bash
# Copyright 2020 Huawei Technologies Co., Ltd
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
# ============================================================================
# export OFFLINE=1 则编译离线版本
# export NO_SFW=1 则不编译hilensframework

BASE_PATH=$(cd `dirname $0`;pwd)

export CC="aarch64-linux-gnu-gcc"
export CXX="aarch64-linux-gnu-g++"

parse_args()
{
    export DEBUGBUILD="Release"
    for var in $@
    do
        if [[ $var == "--offline" ]]
        then
            export OFFLINE=1
        elif [[ $var == "--debug" ]]
        then
            export DEBUGBUILD="Debug"
        else
            echo "Incorrect arguments: " $var
            return 1
        fi
    done
}

build_sfw()
{
    export DDK_Version="887"
    export log4cpluslib="log4cplus"
    export JsonLib="jsoncpp" 
    
    echo "Generating python warpper............................................"
    cd ${BASE_PATH}/python
    swig -c++ -python hilens.i
    if [[ $? != 0 ]];then
        return $?
    fi
    
    echo "Compiling hilensframework............................................."
    cd ${BASE_PATH}/build
    if [[ $OFFLINE == 1 ]];then
        echo "cmake .. -DSF_OFFLINE=ON -DCLOUD=OFF"
        cmake .. -DSF_OFFLINE=ON -DCLOUD=OFF
    else
        echo "cmake .."
        cmake .. -DSF_OFFLINE=OFF -DCLOUD=OFF
    fi
    make -j8
    if [[ $? != 0 ]];then
        return $?
    fi

    echo "Packing HiLensFramework.tar.gz........................................"
    cd ${BASE_PATH}/build
    rm -rf hilensframework
    mkdir -p hilensframework
    
    rm -rf sfw_package
    mkdir -p sfw_package && cd sfw_package

    mkdir -p configs
    cp -a ${BASE_PATH}/deploy/infer.graph ./configs/
    cp -a ${BASE_PATH}/deploy/install.sh ./configs/
    cp -a ${BASE_PATH}/deploy/logger.conf ./configs/
    cp -a ${BASE_PATH}/deploy/scc.conf ./configs/
    cp -a ${BASE_PATH}/deploy/sfw.conf ./configs/
    if [[ $CLOUD == 1 ]];then
        cp -a ${BASE_PATH}/deploy/decode.graph ./configs/
        cp -a ${BASE_PATH}/deploy/encode.graph ./configs/
    fi

    mkdir -p lib
    cp -rf ${BASE_PATH}/third_party/ddk_${DDK_Version}/lib/host/libopencv_world* ./lib/    # 注意只是拷贝opencv_world
    cp -rf ${BASE_PATH}/third_party/boost/lib/* ./lib/
    cp -rf ${BASE_PATH}/third_party/curl/lib/* ./lib/
    cp -rf ${BASE_PATH}/third_party/ffmpeg/lib/* ./lib/
    cp -rf ${BASE_PATH}/third_party/json-c/lib/* ./lib/
    cp -rf ${BASE_PATH}/third_party/${JsonLib}/lib/* ./lib/
    cp -rf ${BASE_PATH}/third_party/${log4cpluslib}/lib/* ./lib/
    cp -rf ${BASE_PATH}/third_party/nanomsg/lib/* ./lib/
    cp -rf ${BASE_PATH}/third_party/python/lib/* ./lib/
    cp -rf ${BASE_PATH}/third_party/srslibrtmp/lib/* ./lib/
    cp -rf ${BASE_PATH}/third_party/zlog/lib/* ./lib/
    # 拷贝我们自己的库
    cp -rf ${BASE_PATH}/engine/lib/*.so ./lib/
    cp -rf ${BASE_PATH}/hilens_media/lib/*.so ./lib/
    cp -rf ${BASE_PATH}/common/lib/*.so ./lib/
    cp -rf ${BASE_PATH}/hilens_security/lib/* ./lib/
    cp -rf ${BASE_PATH}/build/libhilens/*.so ./lib/
    cp -rf ${BASE_PATH}/build/python/*.so ./lib/
    chmod 755 ./lib/*

    mkdir -p python
    cp -rf ${BASE_PATH}/python/hilens_internal.py ./python/
    cp -rf ${BASE_PATH}/python/hilens.py ./python/


    cp -rf ${BASE_PATH}/VERSION ./

    tar czf HiLensFramework.tar.gz --owner=0 --group=0 *
    mv HiLensFramework.tar.gz ../hilensframework

    cp ${BASE_PATH}/deploy/install.sh ../hilensframework
    
    echo "Packing hilens_kit_sdk................................................"
    mkdir template && cd template
    cp -a ../lib ./
    cp -a ../VERSION ./
    cp -rf ${BASE_PATH}/third_party/ddk_${DDK_Version}/lib/host/* ./lib/
    cp -a ${BASE_PATH}/include ./
    cp -a ${BASE_PATH}/third_party/ddk_${DDK_Version}/include/third_party/opencv/include/* ./include
    cp -a ${BASE_PATH}/third_party/jsoncpp/include/* ./include
    cp -a ${BASE_PATH}/python/hilens.py ./
    tar czf hilens_kit_sdk_${HDA_VERSION}.tar.gz --owner=0 --group=0 *
    mv hilens_kit_sdk_${HDA_VERSION}.tar.gz ../..
}

main()
{
    echo "********************************START********************************"
    # 读取版本号以便通过宏写到程序中
    read ver < ${BASE_PATH}/VERSION
    export FIRMWARE_VERSION=$ver
    export HDA_VERSION=$ver

    cd ${BASE_PATH}
    mkdir -p build

    # 构建hilensframework
    if [[ $NO_SFW != 1 ]];then
        build_sfw
    fi

}

parse_args $@ &&
main
