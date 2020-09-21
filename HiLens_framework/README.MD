# HiLensFramework

[中文版](README_ch.md)  

### Dependencies

python = 3.7.0, swig >= 3.0.12

Install swig to generate python API:

For EulerOS: yum install swig 

Other dependencies:

Please check the third_party directory for details.


### Build

- Build environment  
For Ubuntu-x86, you can build your own compiler environment by downloading the cross compiler tool chain.

    | Hardware Platform | Operating System | tools |
    | :---------------- | :--------------- | :----- |
    | CPU | EulerOS-aarch64 | aarch64-linux-gnu-7.3.0 |
    |  | Ubuntu-x86 | cross compiler tool chain |


- Build  
First, you need to prepare the third-party libraries according to third_party README and third_install.sh.  
Then, build HiLensFramework by running ./build.sh. The output will be placed in ./build.

- Cross Compilation Guide  
Download the cross compilation tool at https://hilens-framework-sdk-demo.obs.cn-north-1.myhuaweicloud.com/cross-compile/aarch64-linux-gnu-gcc-7.3.0.zip.  
Directly decompress the package and use the cross compilation tool chain to compile the third-party dependency libraries. Alternatively, you can directly use the precompiled libraries. (For details, see the third_party directory. You are advised to use the compiled libraries for cross compilation.)  
Change the paths of export CC and export CXX in build.sh to the path of the cross compilation tool chain. For example:  
export CC=".../aarch64-linux-gnu-gcc-7.3.0/bin/aarch64-linux-gnu-gcc"  
export CXX=".../aarch64-linux-gnu-gcc-7.3.0/bin/aarch64-linux-gnu-g++"  
Run build.sh to start compilation.


### Install
  Obtain HiLensFramework.tar.gz and install.sh from ./build/hilensframework/ and put them in the /tmp directory in HiLens Kit.  
  Change permissions: chmod +x install.sh.  
  Install the HiLensFramework.tar.gz package: ./install.sh.

### Usages and Examples
The open-source version is based on the official 1.1.0 version. You are advised to upgrade HiLens Kit to 1.1.0 on the HiLens official website before using the open-source version.

- Skills  
For Python skills, you can use the open-source version directly. However, C++ skills need to be recompiled using the hilens_kit_sdk_1.1.0.tar.gz package generated in the build directory. For details, see [the C++ license plate recognition skill and cross compilation guide.](https://support.huaweicloud.com/en-us/sdkreference-hilens/hilens_03_0003.html.)

- Examples
You can directly use the free Python skills in the [Skill Market](https://console.huaweicloud.com/hilens/?region=cn-north-4&locale=en-us#/skillMarket/skillMarketList). We also provide extensive skill templates in [HiLens Studio](https://console.huaweicloud.com/hilens/?region=cn-north-4#/skillDevelop/studioOpening). The source code of a skill template can be viewed online after a skill is created using the template in HiLens Studio. After a Python skill in the skill market is installed on a HiLens Kit camera, you can view the skill source code in the /home/hilens/hda/skill directory of the corresponding Kit camera.

[Skill Market Usage Description](https://support.huaweicloud.com/en-us/usermanual-hilens/hilens_02_0035.html)

[HiLens Studio Usage Description](https://support.huaweicloud.com/en-us/usermanual-hilens/hilens_02_0085.html)

[Developer Guide](https://support.huaweicloud.com/en-us/devg-hilens/hilens_05_0001.html)


### Contact Us
[HiLens Forum](https://bbs.huaweicloud.com/forum/forum-771-1.html) - Communication platform for developers.

## License
[Apache License 2.0](License/LICENSE.txt)
