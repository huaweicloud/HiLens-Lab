cmake_minimum_required(VERSION 2.8)

project(hilens_python)

include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/third_party/numpy/include)
include_directories(${CMAKE_SOURCE_DIR}/third_party/python/include)
include_directories(${CMAKE_SOURCE_DIR}/third_party/jsoncpp/include)
include_directories(${CMAKE_SOURCE_DIR}/third_party/log4cplus/include)
include_directories(${CMAKE_SOURCE_DIR}/third_party/ddk_887/include/libc_sec/include)

include_directories(${CMAKE_SOURCE_DIR}/libhilens)
include_directories(${CMAKE_SOURCE_DIR}/common/include)

add_library(_hilens_internal SHARED hilens_wrap.cxx wrapper.cpp)
target_link_libraries(_hilens_internal hilens python3.7m)
set_target_properties(_hilens_internal PROPERTIES PREFIX "") # 取消_hilens_internal.so的lib前缀
target_compile_options(_hilens_internal PRIVATE -fPIC)
