# HiLensFramework

[中文版](README_ch.md)  

### HiLens Framework Introduction

The HiLens Framework encapsulates bottom-layer APIs to implement common management functions, enabling you to easily develop skills on the Huawei HiLens management console and cultivate the AI ecosystem.

The HiLens Framework encapsulates the bottom-layer multimedia processing libraries \(cameras/microphone driver module Media\_mini\), the image processing library \(DVPP\) related to D chips, and the model management library \(ModelManager\). You can also use the familiar visual processing library OpenCV. In addition, the HiLens Framework provides the following modules for you to develop skills, such as human figure detection, facial recognition, and fatigue driving detection.  [Table 1](#table173537486322)  details the modules. For more information, see [Developer Guide](https://support.huaweicloud.com/en-us/devg-hilens/hilens_05_0001.html).

**Table  1**  Module description 

<a name="table173537486322"></a>
<table><thead align="left"><tr id="row193536481323"><th class="cellrowborder" valign="top" width="6.09%" id="mcps1.2.4.1.1"><p id="p1535319486328"><a name="p1535319486328"></a><a name="p1535319486328"></a><strong id="b298462172615"><a name="b298462172615"></a><a name="b298462172615"></a>No.</strong></p>
</th>
<th class="cellrowborder" valign="top" width="23.95%" id="mcps1.2.4.1.2"><p id="p123531048123214"><a name="p123531048123214"></a><a name="p123531048123214"></a><strong id="b1627195918476"><a name="b1627195918476"></a><a name="b1627195918476"></a>Module</strong></p>
</th>
<th class="cellrowborder" valign="top" width="69.96%" id="mcps1.2.4.1.3"><p id="p4353144873214"><a name="p4353144873214"></a><a name="p4353144873214"></a><strong id="b166939314811"><a name="b166939314811"></a><a name="b166939314811"></a>Function</strong></p>
</th>
</tr>
</thead>
<tbody><tr id="row53539488325"><td class="cellrowborder" valign="top" width="6.09%" headers="mcps1.2.4.1.1 "><p id="p735310482327"><a name="p735310482327"></a><a name="p735310482327"></a>1</p>
</td>
<td class="cellrowborder" valign="top" width="23.95%" headers="mcps1.2.4.1.2 "><p id="p1335394820321"><a name="p1335394820321"></a><a name="p1335394820321"></a>Input Manager</p>
</td>
<td class="cellrowborder" valign="top" width="69.96%" headers="mcps1.2.4.1.3 "><p id="p183538487328"><a name="p183538487328"></a><a name="p183538487328"></a>Input module: manages the access of input data such as video and audio data.</p>
</td>
</tr>
<tr id="row5353174810321"><td class="cellrowborder" valign="top" width="6.09%" headers="mcps1.2.4.1.1 "><p id="p1535312484323"><a name="p1535312484323"></a><a name="p1535312484323"></a>2</p>
</td>
<td class="cellrowborder" valign="top" width="23.95%" headers="mcps1.2.4.1.2 "><p id="p163534482329"><a name="p163534482329"></a><a name="p163534482329"></a>Media Processor</p>
</td>
<td class="cellrowborder" valign="top" width="69.96%" headers="mcps1.2.4.1.3 "><p id="p11353194853210"><a name="p11353194853210"></a><a name="p11353194853210"></a>Preprocessing module: processes media data such as video and audio data.</p>
</td>
</tr>
<tr id="row235314481328"><td class="cellrowborder" valign="top" width="6.09%" headers="mcps1.2.4.1.1 "><p id="p12353948103213"><a name="p12353948103213"></a><a name="p12353948103213"></a>3</p>
</td>
<td class="cellrowborder" valign="top" width="23.95%" headers="mcps1.2.4.1.2 "><p id="p0353248173216"><a name="p0353248173216"></a><a name="p0353248173216"></a>Model Manager</p>
</td>
<td class="cellrowborder" valign="top" width="69.96%" headers="mcps1.2.4.1.3 "><p id="p123539485325"><a name="p123539485325"></a><a name="p123539485325"></a>Model management module: initializes models and infers based on the models.</p>
</td>
</tr>
<tr id="row9353204813323"><td class="cellrowborder" valign="top" width="6.09%" headers="mcps1.2.4.1.1 "><p id="p1435324818328"><a name="p1435324818328"></a><a name="p1435324818328"></a>4</p>
</td>
<td class="cellrowborder" valign="top" width="23.95%" headers="mcps1.2.4.1.2 "><p id="p43532481329"><a name="p43532481329"></a><a name="p43532481329"></a>Output Manager</p>
</td>
<td class="cellrowborder" valign="top" width="69.96%" headers="mcps1.2.4.1.3 "><p id="p183531748173216"><a name="p183531748173216"></a><a name="p183531748173216"></a>Output module: manages output tasks such as streams, files, and message notifications.</p>
</td>
</tr>
<tr id="row935344814321"><td class="cellrowborder" valign="top" width="6.09%" headers="mcps1.2.4.1.1 "><p id="p735384873216"><a name="p735384873216"></a><a name="p735384873216"></a>5</p>
</td>
<td class="cellrowborder" valign="top" width="23.95%" headers="mcps1.2.4.1.2 "><p id="p14353124863219"><a name="p14353124863219"></a><a name="p14353124863219"></a>Resource Manager</p>
</td>
<td class="cellrowborder" valign="top" width="69.96%" headers="mcps1.2.4.1.3 "><p id="p135394811329"><a name="p135394811329"></a><a name="p135394811329"></a>Resource management module: manages paths of resources such as files, images, and models.</p>
</td>
</tr>
<tr id="row935364811328"><td class="cellrowborder" valign="top" width="6.09%" headers="mcps1.2.4.1.1 "><p id="p6353194811323"><a name="p6353194811323"></a><a name="p6353194811323"></a>6</p>
</td>
<td class="cellrowborder" valign="top" width="23.95%" headers="mcps1.2.4.1.2 "><p id="p1235384893217"><a name="p1235384893217"></a><a name="p1235384893217"></a>Logging System</p>
</td>
<td class="cellrowborder" valign="top" width="69.96%" headers="mcps1.2.4.1.3 "><p id="p203531848123217"><a name="p203531848123217"></a><a name="p203531848123217"></a>Log module: manages the log system.</p>
</td>
</tr>
</tbody>
</table>

### Dependencies

swig >= 3.0.12

Install swig to generate python API:

For EulerOS: yum install swig 

Other dependencies:

Please check the third_party directory for details.


### Build

- x86 platform  
For Ubuntu-x86, you can build your own compiler environment by downloading the cross compiler tool chain.

    | Hardware Platform | Operating System | tools |
    | :---------------- | :--------------- | :----- |
    | CPU | Ubuntu-x86 | cross compiler tool chain |

- Cross compilation guide for x86 platform   
Download the cross compilation tool at https://hilens-framework-sdk-demo.obs.cn-north-1.myhuaweicloud.com/cross-compile/aarch64-linux-gnu-gcc-7.3.0.zip.  
Directly decompress the package and use the cross compilation tool chain to compile the third-party dependency libraries. Alternatively, you can directly use the precompiled libraries. (For details, see the third_party directory. You are advised to use the compiled libraries for cross compilation.)  
Change the paths of export CC and export CXX in build.sh to the path of the cross compilation tool chain. For example:  
export CC=".../aarch64-linux-gnu-gcc-7.3.0/bin/aarch64-linux-gnu-gcc"  
export CXX=".../aarch64-linux-gnu-gcc-7.3.0/bin/aarch64-linux-gnu-g++"  
Run build.sh to start compilation.

- aarch64 platform  

    | Hardware Platform | Operating System | tools |
    | :---------------- | :--------------- | :----- |
    | CPU | EulerOS-aarch64 | aarch64-linux-gnu-7.3.0 |

- Compilation guide for aarch64 platform  
First, you need to prepare the third-party libraries according to third_party README and third_install.sh.  
Then, build HiLensFramework by running ./build.sh. The output will be placed in ./build.


### Install
  Obtain HiLensFramework.tar.gz and install.sh from ./build/hilensframework/ and put them in the /tmp directory in HiLens Kit.  
  Change permissions: chmod +x install.sh.  
  Install the HiLensFramework.tar.gz package: ./install.sh.

### Usages and Examples
The open-source version is based on the official 1.1.0 version. You are advised to upgrade HiLens Kit to 1.1.0 on the HiLens official website before using the open-source version.

- Skills  
For Python skills, you can use the open-source version directly. However, C++ skills need to be recompiled using the hilens_kit_sdk_1.1.0.tar.gz package generated in the build directory. For details, see [the C++ license plate recognition skill and cross compilation guide](https://support.huaweicloud.com/en-us/sdkreference-hilens/hilens_03_0003.html).

- Examples  
We provide extensive skill templates in [HiLens Studio](https://console.huaweicloud.com/hilens/?region=cn-north-4#/skillDevelop/studioOpening). The source code of a skill template can be viewed online after a skill is created using the template in HiLens Studio. You can also download the skill templates in [Skill Development - Skill Templates](https://console.huaweicloud.com/hilens/?region=cn-north-4&locale=en-us#/dashboard).

[Obtaining Skill Templates](https://support.huaweicloud.com/en-us/usermanual-hilens/hilens_02_0023.html)

[HiLens Studio Usage Description](https://support.huaweicloud.com/en-us/usermanual-hilens/hilens_02_0085.html)

[Developer Guide](https://support.huaweicloud.com/en-us/devg-hilens/hilens_05_0001.html)


### Contact Us
[HiLens Forum](https://bbs.huaweicloud.com/forum/forum-771-1.html) - Communication platform for developers.

## License
[Apache License 2.0](License/LICENSE.txt)
