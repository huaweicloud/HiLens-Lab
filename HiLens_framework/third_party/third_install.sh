#!/bin/bash

BUILD_PATH=$(cd `dirname $0`;pwd)

wget -P ./ffmpeg http://www.ffmpeg.org/releases/ffmpeg-4.3.1.tar.gz
wget -P ./boost https://dl.bintray.com/boostorg/release/1.72.0/source/boost_1_72_0.tar.gz
wget -P ./curl https://curl.haxx.se/download/curl-7.71.1.tar.gz
wget -P ./json-c https://github.com/json-c/json-c/archive/json-c-0.13.1-20180305.tar.gz
wget -P ./jsoncpp https://github.com/open-source-parsers/jsoncpp/archive/1.9.1.tar.gz
wget -P ./log4cplus https://github.com/log4cplus/log4cplus/releases/download/REL_2_0_4/log4cplus-2.0.4.tar.gz
wget -P ./nanomsg https://github.com/nanomsg/nanomsg/archive/1.1.5.tar.gz
wget -P ./openssl https://github.com/openssl/openssl/archive/OpenSSL_1_1_1g.tar.gz
wget -P ./srsrtmp https://github.com/ossrs/srs/archive/v3.0-r0.tar.gz
wget -P ./websocketpp https://github.com/zaphoyd/websocketpp/archive/0.8.2.tar.gz
wget -P ./zlog https://github.com/HardySimpson/zlog/archive/1.2.14.tar.gz

wget -P ./numpy https://developer.huawei.com/ict/site-euleros/euleros/repo/yum/2.8/os/aarch64/updates/python3-numpy-1.15.1-1.eulerosv2r8.aarch64.rpm
wget -P ./python https://developer.huawei.com/ict/site-euleros/euleros/repo/yum/2.8/os/aarch64/Packages/python3-libs-3.7.0-9.h4.eulerosv2r8.aarch64.rpm
wget -P ./python https://www.python.org/ftp/python/3.7.3/Python-3.7.3.tar.xz
wget -P ./python https://developer.huawei.com/ict/site-euleros/euleros/repo/yum/2.8/os/aarch64/Packages/glibc-2.28-9.h12.eulerosv2r8.aarch64.rpm

wget -P ./ddk/ https://github.com/opencv/opencv/archive/3.4.2.tar.gz

# python and numpy
cd numpy
mkdir include
rpm2cpio python3-numpy*.rpm| cpio -div
cp -rf usr/lib64/python3.7/site-packages/numpy/core/include/* ./include/
rm -rf usr

cd ../python/
tar -xvJf Python-3.7.3.tar.xz
mkdir include && mkdir lib
cp -rf Python-3.7.3/Include/*.h ./include/
rm -rf Python-3.7.3

mkdir glibc && cd glibc
rpm2cpio ../glibc*.rpm| cpio -div
cp -rf lib64/libanl* ../lib
cd ..
rm -rf glibc

mkdir python-lib && cd python-lib
rpm2cpio ../python3*.rpm| cpio -div
cp -rf usr/lib64/libpython* ../lib
cd ..
rm -rf python-lib
cd ..

# websocketpp
cd websocketpp
mkdir include
tar zxvf websocketpp*.tar.gz
cp -rf websocketpp-0.8.2/websocketpp ./include/
rm -rf websocketpp-0.8.2
cd ..

# build boost
cd boost
mkdir lib && mkdir include
mkdir build && cd build
tar -zxvf ../boost
tar zxvf ../boost_1_72_0.tar.gz
cd boost_1_72_0/
./bootstrap.sh
./b2 -j8 cflags=-fstack-protector-strong linkflags=-Wl,-z,relro,-z,now,-z,noexecstack
cp -rf stage/lib/libboost_system.so* ./../../lib/
cp -rf stage/lib/libboost_thread.so* ./../../lib/
cp -rf boost/ ./../../include/
cd ../..
rm -rf build
cd ..

# build openssl
cd openssl
mkdir lib && mkdir include
mkdir build && cd build
tar zxvf ../openssl*.tar.gz
cd openssl*/
./config shared --prefix=${BUILD_PATH}/openssl/build/lib/openssl/ --openssldir=${BUILD_PATH}/openssl/build/lib/openssl/ssl/
make -j8 && make install
cd ../..
cp -rf build/lib/openssl/include/* ./include
cp -rf build/lib/openssl/lib/*.so* ./lib
cd ..

# build curl
cd curl
mkdir lib && mkdir include
mkdir build && cd build
tar zxvf ../curl*.tar.gz
cd curl*/
# ./buildconf如果提示xxx not found可以使用yum安装相应库
./buildconf  
./configure --prefix=${BUILD_PATH}/curl/build/lib/curl --with-ssl=${BUILD_PATH}/openssl/build/lib/openssl/ LDFLAGS='-Wl,-z,relro,-z,now,-z,noexecstack' CFLAGS='-fstack-protector-strong'
make -j8 && make install
cd ../..
cp -rf build/lib/curl/include/* ./include
cp -rf build/lib/curl/lib/*.so* ./lib
rm -rf build
cd ..

# build ffmpeg
cd ffmpeg
mkdir lib && mkdir include
mkdir build && cd build
tar zxvf ../ffmpeg*.tar.gz
cd ffmpeg*/
./configure --enable-shared --disable-static --enable-avresample --prefix=${BUILD_PATH}/ffmpeg/build/lib/ffmpeg/ --extra-cflags=-fstack-protector-strong --extra-ldsoflags=-Wl,-z,relro,-z,now,-z,noexecstack --disable-encoder=tiff --disable-decoder=ilbc --disable-decoder=tiff --disable-encoder=vorbis --disable-decoder=vorbis --disable-decoder=opus --disable-encoder=opus --disable-muxer=opus --disable-parser=opu
make -j8 && make install
cd ../..
cp -rf build/lib/ffmpeg/include/* ./include
cp -rf build/lib/ffmpeg/lib/*.so* ./lib
rm -rf build
cd ..

# build json-c
cd json-c
mkdir lib && mkdir include
mkdir build && cd build
tar zxvf ../json-c*.tar.gz
cd json-c*/
CFLAGS='-fstack-protector-strong' LDFLAGS='-Wl,-z,relro,-z,now,-z,noexecstack'  ./configure --prefix=${BUILD_PATH}/json-c/build/lib/json-c/ 
make -j8 && make install
cd ../..
cp -rf build/lib/json-c/include/* ./include
cp -rf build/lib/json-c/lib/*.so* ./lib
rm -rf build
cd ..

# build zlog
cd zlog
mkdir lib && mkdir include
mkdir build && cd build
tar zxvf ../zlog*.tar.gz
cd zlog*/
make -j8
cp -rf src/zlog.h ../../include
cp -rf src/libzlog.so* ../../lib
cd ../..
rm -rf build
cd ..

# build nanomsg
cd nanomsg
mkdir lib && mkdir include
mkdir build && cd build
tar zxvf ../nanomsg*.tar.gz
cmake nanomsg*/
make -j8
cd ..
cp -rf build/nanomsg-1.1.5/src/*.h ./include
cp -rf build/libnanomsg.so* ./lib
rm -rf build
cd ..

# build log4cplus
#cd log4cplus
#mkdir lib && mkdir include
#mkdir build && cd build
#tar zxvf ../log4cplus*.tar.gz
#cd log4cplus*/
# DDK887需要加上-D_GLIBCXX_USE_CXX11_ABI=0编译选项
# 默认编译的时候带atomic，但是kit上默认没有安装，建议去掉configure.ac中406行AC_SEARCH_LIBS([__atomic_fetch_and_4], [atomic])再运行./configure进行编译配置，参考：https://github.com/log4cplus/log4cplus/commit/b73d961d9500b2e4b63a9a4b4b133deb500922fd
# 或者安装一下atomic：yum install atomic
#CXXFLAGS='-fstack-protector-strong -D_GLIBCXX_USE_CXX11_ABI=0' CFLAGS='-fstack-protector-strong' LDFLAGS='-Wl,-z,relro,-z,now,-z,noexecstack' ./configure --prefix=${BUILD_PATH}/log4cplus/build/lib/log4cplus/
#make -j8 && make install
#cd ../..
#cp -rf build/lib/log4cplus/include/* ./include
#cp -rf build/lib/log4cplus/lib/*.so* ./lib
#rm -rf build
#cd ..

# build srslibrtmp
#cd srslibrtmp
#mkdir lib && mkdir include
#mkdir build && cd build
#tar zxvf ../srs*.tar.gz
#cd srs*/trunk/
#./configure --with-ssl --export-librtmp-single=${BUILD_PATH}/srslibrtmp/build/lib/
#cd ../../lib/
# 这里直接编译会报错，需要修改37252行(param == " ?vhost= "SRS_CONSTS_RTMP_DEFAULT_VHOST)为(param == " ?vhost= " SRS_CONSTS_RTMP_DEFAULT_VHOST)
#g++ srs_librtmp.cpp -fPIC -Wl,-z,relro,-z,now,-z,noexecstack -shared -O3 -o libsrslibrtmp.so -I${BUILD_PATH}/openssl/include/ -L${BUILD_PATH}/openssl/lib/ -lssl
#cd ../..
#cp -rf build/lib/srs_librtmp.h ./include/
#cp -rf build/lib/libsrslibrtmp.so ./lib/
#rm -rf build
#cd ..

# build Jsoncpp 
#cd jsoncpp
#mkdir lib && mkdir include
#mkdir build && cd build
#tar zxvf ../jsoncpp*.tar.gz
#cd jsoncpp*/
# 修改CMakelist.txt，修改以下两项使编译生成共享库：
# OPTION(BUILD_SHARED_LIBS "Build jsoncpp_lib as a shared library." ON)
# OPTION(BUILD_STATIC_LIBS "Build jsoncpp_lib static library." OFF)
# 使用B887版本DDK的HOST侧库需要在CMakelist.txt加上此编译选项
# add_compile_options(-D_GLIBCXX_USE_CXX11_ABI=0)
#vi CMakelist.txt
#cd ../../build/
#cmake jsoncpp-*/
#make  -j8
#cd ..
#cp -rf build/src/lib_json/libjsoncpp.so* ./lib/
#cp -rf build/jsoncpp-*/include/json ./include/
#rm -rf build
#cd ..

echo "Build finished!"




