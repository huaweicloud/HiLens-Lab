# HiLensFramework

[中文版](README_ch.md)  

#### Directories/files

| Directories/files | instruction                                                 |
| ----------------- | ----------------------------------------------------------- |
| third_party       | Dependencies(Huawei self-developed and third-party)         |
| build             | To place our output                                         |
| deploy            | Service, configs                                            |
| engine            | AI engines(including device and host)                       |
| hilens_media      | Media libaray (3559 mpp)                                    |
| hilens_security   | Some internal tools of HiLensFramework                      |
| include           | C++ API of SDK                                              |
| libhilens         | Main lib of HiLensFramework                                 |
| python            | Python API of SDK                                           |
| VERSION           | Version of HiLensFramework                                  |
| License           | License and open-source software notice                     |

### Dependencies

Install swig to generate python API:

For EulerOS: yum install swig 

Other dependencies:

Please check the third_party directory for details.


### Build

- Build environment  
For Ubuntu-x86, you can build your own compiler environment by downloading the cross compiler tool chain. Visit https://support.huaweicloud.com/sdkreference-hilens/hilens_03_0003.html for more information.

    | Hardware Platform | Operating System | tools |
    | :---------------- | :--------------- | :----- |
    | CPU | EulerOS-aarch64 | aarch64-linux-gnu-7.3.0 |
    |  | Ubuntu-x86 | cross compiler tool chain |


- Build  
First, you need to prepare the third-party libraries according to third_party README and third_install.sh.  
Then, build HiLensFramework by running ./build.sh. The output will be placed in ./build.

### Install
  Obtain HiLensFramework.tar.gz and install.sh from ./build/hilensframework/ and put them in the /tmp directory in HiLens Kit.  
  Change permissions: chmod +x install.sh.  
  Install the HiLensFramework.tar.gz package: ./install.sh.

### Use
The open-source Version is based on the formal 1.1.0 version.

- App  
For python apps, you can use them as usual. However, C++ apps need to be compiled with the open-source SDK in ./build/hilens_kit_sdk_1.1.0.tar.gz.

### Contact Us
[HiLens Forum](https://bbs.huaweicloud.com/forum/forum-771-1.html) - Communication platform for developers.

## License
[Apache License 2.0](License/LICENSE.txt)
