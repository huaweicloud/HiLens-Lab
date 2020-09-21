
### 依赖

- 三方

    | Dependencies      | Version                                                     |
    | ----------------- | ----------------------------------------------------------- |
    | boost             | 1.7.2                                                       |
    | curl              | 7.71.1                                                      |
    | ffmpeg            | 4.3.1                                                       |
    | json-c            | 0.13.1                                                      |
    | jsoncpp           | 1.9.1                                                       |
    | log4cplus         | 2.0.4                                                       |
    | nanomsg           | 1.1.5                                                       |
    | numpy             | 1.5.1(EulerOS-aarch64)                                      |
    | openssl           | 1.1.1g                                                      |
    | python            | 3.7.0(EulerOS-aarch64)                                      |
    | srs               | 3.0-r0                                                      |
    | websocketpp       | 0.8.2                                                       |
    | zlog              | 1.2.14                                                      |
    | opencv            | 3.4.2                                                       |

- 华为

    | Dependencies      | Version                                                     |
    | ----------------- | ----------------------------------------------------------- |
    | DDK               |  A500-3000_A500-3010_A200-3000HiLens-DDK-V2.2.100.031.887   |


### 下载和编译

- 使用third_install.sh下载和编译

    可以直接通过运行third_party目录的third_install.sh脚本来自动下载和编译一部分依赖库。
    也可以参考脚本地址自己下载相关的包。

    下载的库：boost  curl  ffmpeg  json-c  jsoncpp  log4cplus  nanomsg  numpy  openssl  python  srslibrtmp websocketpp  zlog  opencv

    编译的库：boost  curl  ffmpeg  json-c  jsoncpp  log4cplus  nanomsg  openssl  srslibrtmp  zlog

- 其他依赖库编译
    log4cplus、srslibrtmp、Jsoncpp由于不能直接生成我们需要的库，所以需要手动编译。

    手动编译指导可以参考third_install.sh脚本最后注释部分。

    #### 注意：如果使用A500-3000_A500-3010_A200-3000HiLens-DDK-V2.2.100.031.887版本的DDK，编译log4cplus和Jsoncpp的时候需要加上-D_GLIBCXX_USE_CXX11_ABI=0编译选项

- DDK可以通过以下链接进行下载

    [A500-3000_A500-3010_A200-3000HiLens-DDK-V2.2.100.031.887](https://hilens-framework-sdk-demo.obs.cn-north-1.myhuaweicloud.com/DDK/A500-3000_A500-3010_A200-3000HiLens-DDK-V2.2.100.031.887.tar.gz
    )

    下载之后直接解压，拷贝相关头文件和库按以下目录放置, opencv头文件可以使用上面下载的opencv包中的头文件：
    ```
    ddk_887/
    |- include/
    |   |- inc/
    |   |- libc_sec/include
    |   |- third_party/
    |       |- cereal/
    |       |- gflags/
    |       |- glog/
    |       |- opencv/
    |       |- protobuf/
    |- lib/
    |   |- device/
    |   |- host/
    ```

- 完整目录结构示意图

    ```
    third_party/
    |- boost/
    |   |- include/
    |   |- lib/
    |- curl/
    |   |- include/
    |   |- lib/
    |- ffmpeg/
    |   |- include/
    |   |- lib/
    |- json-c/
    |   |- include/
    |   |- lib/
    |- jsoncpp/
    |   |- include/
    |   |- lib/
    |- log4cplus/
    |   |- include/
    |   |- lib/
    |- nanomsg/
    |   |- include/
    |   |- lib/
    |- numpy/
    |   |- include/
    |   |- lib/
    |- openssl/
    |   |- include/
    |   |- lib/
    |- python/
    |   |- include/
    |   |- lib/
    |- srslibrtmp/
    |   |- include/
    |   |- lib/
    |- websocketpp/
    |   |- include/
    |   |- lib/
    |- zlog/
    |   |- include/
    |   |- lib/
    |- ddk_887/
    |   |- include/
    |   |- lib/
    ```

## 预编译的库

如果不想自己编译的话也可以直接使用我们预编译整理好的库：

[third_party_prebuild](https://hilens-framework-sdk-demo.obs.cn-north-1.myhuaweicloud.com/open-source/third_party_prebuild.tar.gz)