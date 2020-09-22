# HiLensFramework

### 简介


HiLens Framework通过封装底层接口、实现常用的管理功能，让开发者可以在Huawei HiLens管理控制台上方便地开发技能，培育AI生态。

HiLens Framework封装了底层的多媒体处理库（摄像头/麦克风驱动模块Media\_mini），以及D芯片相关的图像处理库（DVPP）和模型管理库（ModelManager），另外开发者也可以使用熟悉的视觉处理库OpenCV。在此之上，HiLens Framework提供了以下6个模块供开发者使用，方便开发诸如人形检测、人脸识别、疲劳驾驶检测等技能，模块说明如[表1](#table173537486322)所示。详细介绍请查看[开发指南](https://support.huaweicloud.com/devg-hilens/hilens_05_0001.html)。


**表 1**  模块说明

<a name="table173537486322"></a>
<table><thead align="left"><tr id="row193536481323"><th class="cellrowborder" valign="top" width="6.09%" id="mcps1.2.4.1.1"><p id="p1535319486328"><a name="p1535319486328"></a><a name="p1535319486328"></a>序号</p>
</th>
<th class="cellrowborder" valign="top" width="23.95%" id="mcps1.2.4.1.2"><p id="p123531048123214"><a name="p123531048123214"></a><a name="p123531048123214"></a>模块</p>
</th>
<th class="cellrowborder" valign="top" width="69.96%" id="mcps1.2.4.1.3"><p id="p4353144873214"><a name="p4353144873214"></a><a name="p4353144873214"></a>功能</p>
</th>
</tr>
</thead>
<tbody><tr id="row53539488325"><td class="cellrowborder" valign="top" width="6.09%" headers="mcps1.2.4.1.1 "><p id="p735310482327"><a name="p735310482327"></a><a name="p735310482327"></a>1</p>
</td>
<td class="cellrowborder" valign="top" width="23.95%" headers="mcps1.2.4.1.2 "><p id="p1335394820321"><a name="p1335394820321"></a><a name="p1335394820321"></a>Input Manager</p>
</td>
<td class="cellrowborder" valign="top" width="69.96%" headers="mcps1.2.4.1.3 "><p id="p183538487328"><a name="p183538487328"></a><a name="p183538487328"></a>输入模块：负责视频、音频等输入数据的接入管理。</p>
</td>
</tr>
<tr id="row5353174810321"><td class="cellrowborder" valign="top" width="6.09%" headers="mcps1.2.4.1.1 "><p id="p1535312484323"><a name="p1535312484323"></a><a name="p1535312484323"></a>2</p>
</td>
<td class="cellrowborder" valign="top" width="23.95%" headers="mcps1.2.4.1.2 "><p id="p163534482329"><a name="p163534482329"></a><a name="p163534482329"></a>Media Processor</p>
</td>
<td class="cellrowborder" valign="top" width="69.96%" headers="mcps1.2.4.1.3 "><p id="p11353194853210"><a name="p11353194853210"></a><a name="p11353194853210"></a>预处理模块：负责视频、音频等媒体数据的处理。</p>
</td>
</tr>
<tr id="row235314481328"><td class="cellrowborder" valign="top" width="6.09%" headers="mcps1.2.4.1.1 "><p id="p12353948103213"><a name="p12353948103213"></a><a name="p12353948103213"></a>3</p>
</td>
<td class="cellrowborder" valign="top" width="23.95%" headers="mcps1.2.4.1.2 "><p id="p0353248173216"><a name="p0353248173216"></a><a name="p0353248173216"></a>Model Manager</p>
</td>
<td class="cellrowborder" valign="top" width="69.96%" headers="mcps1.2.4.1.3 "><p id="p123539485325"><a name="p123539485325"></a><a name="p123539485325"></a>模型管理模块：负责模型的初始化与推断任务。</p>
</td>
</tr>
<tr id="row9353204813323"><td class="cellrowborder" valign="top" width="6.09%" headers="mcps1.2.4.1.1 "><p id="p1435324818328"><a name="p1435324818328"></a><a name="p1435324818328"></a>4</p>
</td>
<td class="cellrowborder" valign="top" width="23.95%" headers="mcps1.2.4.1.2 "><p id="p43532481329"><a name="p43532481329"></a><a name="p43532481329"></a>Output Manager</p>
</td>
<td class="cellrowborder" valign="top" width="69.96%" headers="mcps1.2.4.1.3 "><p id="p183531748173216"><a name="p183531748173216"></a><a name="p183531748173216"></a>输出模块：负责流、文件、消息通知等输出任务的管理。</p>
</td>
</tr>
<tr id="row935344814321"><td class="cellrowborder" valign="top" width="6.09%" headers="mcps1.2.4.1.1 "><p id="p735384873216"><a name="p735384873216"></a><a name="p735384873216"></a>5</p>
</td>
<td class="cellrowborder" valign="top" width="23.95%" headers="mcps1.2.4.1.2 "><p id="p14353124863219"><a name="p14353124863219"></a><a name="p14353124863219"></a>Resource Manager</p>
</td>
<td class="cellrowborder" valign="top" width="69.96%" headers="mcps1.2.4.1.3 "><p id="p135394811329"><a name="p135394811329"></a><a name="p135394811329"></a>资源管理模块：负责文件、图片、模型等资源的路径管理。</p>
</td>
</tr>
<tr id="row935364811328"><td class="cellrowborder" valign="top" width="6.09%" headers="mcps1.2.4.1.1 "><p id="p6353194811323"><a name="p6353194811323"></a><a name="p6353194811323"></a>6</p>
</td>
<td class="cellrowborder" valign="top" width="23.95%" headers="mcps1.2.4.1.2 "><p id="p1235384893217"><a name="p1235384893217"></a><a name="p1235384893217"></a>Logging System</p>
</td>
<td class="cellrowborder" valign="top" width="69.96%" headers="mcps1.2.4.1.3 "><p id="p203531848123217"><a name="p203531848123217"></a><a name="p203531848123217"></a>日志模块：负责日志系统管理。</p>
</td>
</tr>
</tbody>
</table>


### 依赖

swig >= 3.0.12

生成python接口需要安装swig:

EulerOS可以直接使用yum安装: yum install swig  
安装完成后可以通过swig -version查看版本


其他依赖:

请按照third_party目录的说明和脚本进行下载和使用。


### 编译

- x86平台  
    | 硬件平台 | 操作系统 | 工具链 |
    | :---------------- | :--------------- | :----- |
    | CPU | Ubuntu-x86 | 交叉编译工具链 |

- x86平台编译  
下载[交叉编译工具](https://hilens-framework-sdk-demo.obs.cn-north-1.myhuaweicloud.com/cross-compile/aarch64-linux-gnu-gcc-7.3.0.zip)，直接解压使用。  
使用交叉编译工具链编译第三方依赖库，或者直接将预编译好的库拷贝到third_party目录使用(详见third_party目录，交叉编译建议直接使用预编译好的库)。  
修改build.sh中export CC和export CXX的路径为交叉编译工具链路径，如：  
export CC=".../aarch64-linux-gnu-gcc-7.3.0/bin/aarch64-linux-gnu-gcc"  
export CXX=".../aarch64-linux-gnu-gcc-7.3.0/bin/aarch64-linux-gnu-g++"  
直接运行build.sh开始编译。

- aarch64平台  

    | 硬件平台 | 操作系统 | 工具链 |
    | :---------------- | :--------------- | :----- |
    | CPU | EulerOS-aarch64 | aarch64-linux-gnu-7.3.0 |


- aarch64编译  
首先，按照third_party目录的README和third_install.sh准备好三方依赖库。 
然后, 直接运行./build.sh来编译HiLensFramework。输出产物将在./output目录生成。  


### 安装
  从./build/hilensframework/目录获取HiLensFramework.tar.gz安装包和install.sh安装脚本，并将它们放置在HiLens Kit的/tmp目录。  
  给予运行权限: chmod +x install.sh。  
  安装HiLensFramework.tar.gz: ./install.sh。

### 使用及示例
开源版本基于正式的1.1.0版本。建议在HiLens官网上把HiLens Kit升级到1.1.0版本再使用开源版本。

- 技能  
对于python技能来说, 可以直接使用开源版本。 但是C++技能需要用build目录生成的hilens_kit_sdk_1.1.0.tar.gz包重新编译，可以参考[c++车牌识别技能及交叉编译指导](https://support.huaweicloud.com/sdkreference-hilens/hilens_03_0003.html)。

- 示例  
[HiLens Studio](https://console.huaweicloud.com/hilens/?region=cn-north-4#/skillDevelop/studioOpening)中提供了丰富的技能模板，你可以从模板新建技能直接在线查看，你也可以通过[技能开发-技能模板](https://console.huaweicloud.com/hilens/?region=cn-north-4#/skillDevelop/projectTemplate)页面直接下载对应的技能模板查看和使用。

[获取技能模板](https://support.huaweicloud.com/usermanual-hilens/hilens_02_0023.html)

[HiLens Studio使用说明](https://support.huaweicloud.com/usermanual-hilens/hilens_02_0085.html)

[开发指南](https://support.huaweicloud.com/devg-hilens/hilens_05_0001.html)

### 联系我们
[HiLens 论坛](https://bbs.huaweicloud.com/forum/forum-771-1.html) - 开发者交流平台。

## License
[Apache License 2.0](License/LICENSE.txt)
