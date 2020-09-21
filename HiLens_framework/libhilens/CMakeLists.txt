cmake_minimum_required(VERSION 2.8)

project(hilens)

# 当CLOUD为ON时，编译Studio版本
if(CLOUD)
    message("\nBuilding Framework for studio...\n")
    add_definitions(-DCLOUD)
endif()

if(CLOUD)
set(DDK_Version "893")
set(DDK_ExtraLib graph hiai_common)
else()
# 使用B887版本DDK的HOST侧库需要加上此编译选项
add_compile_options(-D_GLIBCXX_USE_CXX11_ABI=0)
set(DDK_Version "887")
set(DDK_ExtraLib "")
endif()

if(CMAKE_BUILD_TYPE MATCHES DEBUG)
set(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g -ggdb")
endif()


include_directories(${CMAKE_SOURCE_DIR}/third_party/zlog/include)
include_directories(${CMAKE_SOURCE_DIR}/third_party/numpy/include)
include_directories(${CMAKE_SOURCE_DIR}/third_party/srslibrtmp/include)
include_directories(${CMAKE_SOURCE_DIR}/third_party/python/include)
include_directories(${CMAKE_SOURCE_DIR}/third_party/curl/include)
include_directories(${CMAKE_SOURCE_DIR}/third_party/ffmpeg/include)
include_directories(${CMAKE_SOURCE_DIR}/third_party/boost/include)
include_directories(${CMAKE_SOURCE_DIR}/third_party/jsoncpp/include)
include_directories(${CMAKE_SOURCE_DIR}/third_party/openssl/include)
include_directories(${CMAKE_SOURCE_DIR}/third_party/log4cplus/include)
include_directories(${CMAKE_SOURCE_DIR}/third_party/websocketpp/include)

include_directories(${CMAKE_SOURCE_DIR}/third_party/ddk_${DDK_Version}/include/inc)
include_directories(${CMAKE_SOURCE_DIR}/third_party/ddk_${DDK_Version}/include/libc_sec/include)
include_directories(${CMAKE_SOURCE_DIR}/third_party/ddk_${DDK_Version}/include/third_party/cereal/include)
include_directories(${CMAKE_SOURCE_DIR}/third_party/ddk_${DDK_Version}/include/third_party/opencv/include)
include_directories(${CMAKE_SOURCE_DIR}/third_party/ddk_${DDK_Version}/include/third_party/protobuf/include)

include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/hilens_media/include)
include_directories(${CMAKE_SOURCE_DIR}/common/include)
include_directories(${CMAKE_SOURCE_DIR}/hilens_security/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} SRC_LIST)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/obszilla OBSZILLA_SRC_LIST)

link_directories(${CMAKE_SOURCE_DIR}/third_party/ddk_${DDK_Version}/lib/host)

set(CMAKE_CXX_FLAGS "-Wno-narrowing")

add_library(hilens SHARED ${SRC_LIST} ${OBSZILLA_SRC_LIST})
target_link_libraries(hilens
    ssl crypto 
    hilens_media
    hilens_common
    hilens_security
    log4cplus
    jsoncpp json-c
    curl srslibrtmp nanomsg boost_system
    pthread memory
    swresample avcodec avformat avutil swscale avresample
    c_sec matrix opencv_world slog
    ${DDK_ExtraLib})
target_compile_options(hilens PRIVATE -fPIC)
