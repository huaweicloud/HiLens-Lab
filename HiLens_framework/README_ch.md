# HiLensFramework

### 依赖

python = 3.7.0, swig >= 3.0.12

生成python接口需要安装swig:

EulerOS可以直接使用yum安装: yum install swig 

其他依赖:

请按照third_party目录的说明和脚本进行下载和使用。


### 编译

- 编译环境  
如果使用Ubuntu-x86的话，需要进行交叉编译。  

    | 硬件平台 | 操作系统 | 工具链 |
    | :---------------- | :--------------- | :----- |
    | CPU | EulerOS-aarch64 | aarch64-linux-gnu-7.3.0 |
    |  | Ubuntu-x86 | 交叉编译工具链 |

- 编译  
首先，你需要按照third_party目录的README和third_install.sh准备好三方依赖库。 
然后, 直接运行./build.sh来编译HiLensFramework。输出产物将在./output目录生成。

- 交叉编译使用指导  
下载[交叉编译工具](https://hilens-framework-sdk-demo.obs.cn-north-1.myhuaweicloud.com/cross-compile/aarch64-linux-gnu-gcc-7.3.0.zip)。  
直接解压，使用交叉编译工具链编译三方依赖库，或者直接使用我们已经预编译好的(详见third_party目录，交叉编译建议直接使用我们编译好的库)。  
修改build.sh中export CC和export CXX的路径为交叉编译工具链路径，如：  
export CC=".../aarch64-linux-gnu-gcc-7.3.0/bin/aarch64-linux-gnu-gcc"  
export CXX=".../aarch64-linux-gnu-gcc-7.3.0/bin/aarch64-linux-gnu-g++"  
直接运行build.sh开始编译。


### 安装
  从./build/hilensframework/目录获取HiLensFramework.tar.gz安装包和install.sh安装脚本，并将它们放置在HiLens Kit的/tmp目录。  
  给予运行权限: chmod +x install.sh。  
  安装HiLensFramework.tar.gz: ./install.sh。

### 使用及示例
开源版本基于正式的1.1.0版本。建议在HiLens官网上把HiLens Kit升级到1.1.0版本再使用开源版本。

- 技能  
对于python技能来说, 你可以直接使用开源版本。 但是C++技能需要用build目录生成的hilens_kit_sdk_1.1.0.tar.gz包重新编译，可以参考[c++车牌识别技能及交叉编译指导](https://support.huaweicloud.com/sdkreference-hilens/hilens_03_0003.html)。

- 示例  
你可以直接使用[技能市场](https://console.huaweicloud.com/hilens/?region=cn-north-4#/skillMarket/skillMarketList)的python免费技能，我们也在[HiLens Studio](https://console.huaweicloud.com/hilens/?region=cn-north-4#/skillDevelop/studioOpening)中提供了丰富的技能模板。HiLens Studio技能模板可以从模板新建技能后直接在线查看，技能市场的Python技能安装到HiLens Kit后可以到相应Kit的/home/hilens/hda/skill目录查看技能源代码。  

[技能市场使用说明](https://support.huaweicloud.com/usermanual-hilens/hilens_02_0035.html)

[HiLens Studio使用说明](https://support.huaweicloud.com/usermanual-hilens/hilens_02_0085.html)

[开发指南](https://support.huaweicloud.com/devg-hilens/hilens_05_0001.html)

### 联系我们
[HiLens 论坛](https://bbs.huaweicloud.com/forum/forum-771-1.html) - 开发者交流平台。

## License
[Apache License 2.0](License/LICENSE.txt)
