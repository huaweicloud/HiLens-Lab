# HiLensFramework


#### 目录结构

| 目录/文件          | 描述                                                 |
| ----------------- | ----------------------------------------------------------- |
| third_party       | 依赖(包括华为自研和开源三方)         |
| build             | 编译输出目录                                         |
| deploy            | 配置文件、安装脚本                                            |
| engine            | AI engines(包括device侧和host侧)                       |
| hilens_media      | 媒体库 (3559 mpp)                                    |
| hilens_security   | 内部依赖                      |
| include           | C++接口                                              |
| libhilens         | 主要实现代码                                 |
| python            | Python封装接口                                           |
| VERSION           | 版本号                                  |
| License           | License和open-source software notice                     |

### 依赖

生成python接口需要安装swig:

EulerOS可以直接使用yum安装: yum install swig 

其他依赖:

请按照third_party目录的说明和脚本进行下载和使用。


### 编译

- 编译环境  
如果使用Ubuntu-x86的话，需要使用交叉编译工具, 可以参考https://support.huaweicloud.com/sdkreference-hilens/hilens_03_0003.html 来使用。

    | 硬件平台 | 操作系统 | 工具链 |
    | :---------------- | :--------------- | :----- |
    | CPU | EulerOS-aarch64 | aarch64-linux-gnu-7.3.0 |
    |  | Ubuntu-x86 | 交叉编译工具链 |


- 编译  
首先，你需要按照third_party目录的README和third_install.sh准备好三方依赖库。 
然后, 直接运行./build.sh来编译HiLensFramework。输出产物将在./output目录生成。

### 安装
  从./build/hilensframework/目录获取HiLensFramework.tar.gz安装包和install.sh安装脚本，并将它们放置在HiLens Kit的/tmp目录。  
  给予运行权限: chmod +x install.sh。  
  安装HiLensFramework.tar.gz: ./install.sh。

### 使用
开源版本基于正式的1.1.0版本。建议在HiLens官网上把HiLens Kit升级到1.1.0版本再使用开源版本。

- 技能  
对于python技能来说, 你可以直接使用开源版本。 但是C++技能需要用build目录生成的hilens_kit_sdk_1.1.0.tar.gz包重新编译。

- 示例  
你可以直接使用[技能市场](https://console.huaweicloud.com/hilens/?region=cn-north-4#/skillMarket/skillMarketList)的python免费技能，我们也在[HiLens Studio](https://console.huaweicloud.com/hilens/?region=cn-north-4#/skillDevelop/studioOpening)中提供了丰富的技能模板。  

[技能市场使用说明](https://support.huaweicloud.com/usermanual-hilens/hilens_02_0035.html)

[HiLens Studio使用说明](https://support.huaweicloud.com/usermanual-hilens/hilens_02_0085.html)

[开发指南](https://support.huaweicloud.com/devg-hilens/hilens_05_0001.html)

### 联系我们
[HiLens 论坛](https://bbs.huaweicloud.com/forum/forum-771-1.html) - 开发者交流平台。

## License
[Apache License 2.0](License/LICENSE.txt)
