cmake_minimum_required(VERSION 3.0)

project(SkillFramework)

# 使用C++11标准
set(CMAKE_CXX_STANDARD 11)

#ADD_DEFINITIONS(-g)

# 当CLOUD为ON时，编译Studio版本
if(CLOUD)
    message("\nBuilding Framework for studio...\n")
    add_definitions(-DCLOUD)
endif()

if(CLOUD)
set(DDK_Version "893")
set(JsonLib "jsoncpp-cloud")
set(log4cpluslib "log4cplus-cloud")
else()
# 使用B887版本DDK的HOST侧库需要加上此编译选项
add_compile_options(-D_GLIBCXX_USE_CXX11_ABI=0)
set(DDK_Version "887")
set(JsonLib "jsoncpp")
set(log4cpluslib "log4cplus")
endif()

if(CMAKE_BUILD_TYPE MATCHES DEBUG)
set(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g -ggdb")
endif()

# 添加安全编译选项
add_compile_options(-fstack-protector-strong)
set(CMAKE_SHARED_LINKER_FLAGS "-Wl,-z,relro,-z,now,-z,noexecstack")
set(CMAKE_EXE_LINKER_FLAGS "-Wl,-z,relro,-z,now,-z,noexecstack -pie")

# 不自动添加rpath
set(CMAKE_SKIP_RPATH TRUE)
set(CMAKE_SKIP_BUILD_RPATH TRUE)
set(CMAKE_SKIP_INSTALL_RPATH TRUE)

# 当SFW_OFFLINE为ON时，编译离线版本
if(SF_OFFLINE)
    message("\nBuilding offline package...\n")
    add_definitions(-DSF_OFFLINE)
endif()


include_directories(${CMAKE_SOURCE_DIR}/third_party/openssl/include)
include_directories(${CMAKE_SOURCE_DIR}/third_party/curl/include)
include_directories(${CMAKE_SOURCE_DIR}/third_party/ddk_${DDK_Version}/include/third_party/opencv/include)

link_directories(${CMAKE_SOURCE_DIR}/third_party/ApiGateway/lib/${LIB_DIR})
link_directories(${CMAKE_SOURCE_DIR}/third_party/srslibrtmp/lib/${LIB_DIR})
link_directories(${CMAKE_SOURCE_DIR}/third_party/python/lib/${LIB_DIR})
link_directories(${CMAKE_SOURCE_DIR}/third_party/curl/lib/${LIB_DIR})
link_directories(${CMAKE_SOURCE_DIR}/third_party/ffmpeg/lib/${LIB_DIR})
link_directories(${CMAKE_SOURCE_DIR}/third_party/${JsonLib}/lib/${LIB_DIR})
link_directories(${CMAKE_SOURCE_DIR}/third_party/openssl/lib/${LIB_DIR})
link_directories(${CMAKE_SOURCE_DIR}/third_party/${log4cpluslib}/lib/${LIB_DIR})
link_directories(${CMAKE_SOURCE_DIR}/third_party/nanomsg/lib/${LIB_DIR})
link_directories(${CMAKE_SOURCE_DIR}/third_party/json-c/lib/${LIB_DIR})
link_directories(${CMAKE_SOURCE_DIR}/third_party/zlog/lib/${LIB_DIR})
link_directories(${CMAKE_SOURCE_DIR}/third_party/ddk_${DDK_Version}/lib/host/${LIB_DIR})
link_directories(${CMAKE_SOURCE_DIR}/third_party/boost/lib/${LIB_DIR})
link_directories(${CMAKE_SOURCE_DIR}/hilens_media/lib/${LIB_DIR})
link_directories(${CMAKE_SOURCE_DIR}/common/lib/${LIB_DIR})
link_directories(${CMAKE_SOURCE_DIR}/hilens_security/lib/${LIB_DIR})

add_subdirectory(libhilens)
add_subdirectory(python)
