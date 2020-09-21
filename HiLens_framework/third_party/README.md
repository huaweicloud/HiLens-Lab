
### Dependencies

[中文版](README_ch.md)

- Third party

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

- Slf-developed

    | Dependencies      | Version                                                     |
    | ----------------- | ----------------------------------------------------------- |
    | DDK               |  A500-3000_A500-3010_A200-3000HiLens-DDK-V2.2.100.031.887   |


### Download and Compile

- use third_install.sh to download and compile

    You can run the third_install.sh script in the third_party directory to automatically download and compile some dependencies.
    You can also download related packages according to the script addresses.

    Downloaded libraries：boost  curl  ffmpeg  json-c  jsoncpp  log4cplus  nanomsg  numpy  openssl  python  srslibrtmp websocketpp  zlog  opencv

    Compiled libraries：boost  curl  ffmpeg  json-c  jsoncpp  log4cplus  nanomsg  openssl  srslibrtmp  zlog

- Compilation of other dependencies
    log4cplus, srslibrtmp, and Jsoncpp cannot directly generate the required libraries. Therefore, you need to manually compile them.

    For details about how to manually compile them, see the comments in the third_install.sh script.

    #### Note: If the DDK of the A500-3000_A500-3010_A200-3000HiLens-DDK-V2.2.100.031.887 version is used, you need to add the -D_GLIBCXX_USE_CXX11_ABI=0 compilation option when compiling log4cplus and Jsoncpp.

- The DDK can be downloaded from the following link:

    [A500-3000_A500-3010_A200-3000HiLens-DDK-V2.2.100.031.887](https://hilens-framework-sdk-demo.obs.cn-north-1.myhuaweicloud.com/DDK/A500-3000_A500-3010_A200-3000HiLens-DDK-V2.2.100.031.887.tar.gz
    )

    Decompress the downloaded package and copy the related header files and libraries to the following directories. The opencv header file in the preceding downloaded opencv package can be directly used here.
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

- Structure of the complete directories:

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

## Precompiled libraries

If you do not want to compile the libraries by yourself, you can directly use the precompiled libraries.

[third_party_prebuild](https://hilens-framework-sdk-demo.obs.cn-north-1.myhuaweicloud.com/open-source/third_party_prebuild.tar.gz)
