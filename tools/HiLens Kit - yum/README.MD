## HiLens Kit安装yum及配置yum源

查看系统版本
```
$ uname -a 
```
"Linux Euler 4.19.36-vhulk1907.1.0.h448.eulerosv2r8.aarch64 #1 SMP Mon Jul 22 00:00:00 UTC 2019 aarch64 aarch64 aarch64 GNU/Linux"

下载附件rpm包放到/tmp目录，或者去[华为开源镜像站](https://mirrors.huaweicloud.com/euler/2.8/os/aarch64/Packages/)下载相应rpm包，并用unzip解压，进入目录依次安装以下库：

```
$ rpm -ivh deltarpm-3.6-28.eulerosv2r8.aarch64.rpm dnf-data-4.0.4-1.h2.eulerosv2r8.noarch.rpm python3-iniparse-0.4-32.eulerosv2r8.noarch.rpm python3-libcomps-0.1.8-14.eulerosv2r8.aarch64.rpm libcomps-*.rpm
$ rpm -ivh --force gpgme-1.11.1-3.eulerosv2r8.aarch64.rpm 
$ rpm -ivh python3-gpg-1.11.1-3.eulerosv2r8.aarch64.rpm
$ rpm -ivh libyaml-0.2.1-2.h1.eulerosv2r8.aarch64.rpm libmodulemd-1.6.4-1.eulerosv2r8.aarch64.rpm
$ rpm -ivh python3-hawkey-0.22.0-6.eulerosv2r8.aarch64.rpm libdnf-0.22.0-6.eulerosv2r8.aarch64.rpm librepo-1.9.1-1.eulerosv2r8.aarch64.rpm libsolv-0.6.35-3.eulerosv2r8.aarch64.rpm python3-libdnf-0.22.0-6.eulerosv2r8.aarch64.rpm
$ rpm -ivh ima-evm-utils-1.1-4.eulerosv2r8.aarch64.rpm
$ rpm -ivh --force python3-rpm-4.14.2-1.h5.eulerosv2r8.aarch64.rpm rpm-libs-4.14.2-1.h5.eulerosv2r8.aarch64.rpm rpm-sign-*.rpm rpm-build-libs-4.14.2-1.h5.eulerosv2r8.aarch64.rpm rpm-4.14.2-1.h5.eulerosv2r8.aarch64.rpm
$ rpm -ivh python3-dnf-*.rpm
$ rpm -ivh dnf-4.0.4-1.h2.eulerosv2r8.noarch.rpm
$ rpm -ivh yum-4.0.4-1.h2.eulerosv2r8.noarch.rpm

```
这个时候运行yum还会报缺少以下几个库：

```
ImportError: libgio-2.0.so.0: cannot open shared object file: No such file or directory
ImportError: libnghttp2.so.14: cannot open shared object file: No such file or directory
ImportError: libssh.so.4: cannot open shared object file: No such file or directory
ImportError: libbrotlidec.so.1: cannot open shared object file: No such file or directory

```

需要安装:

```
$ rpm -ivh --force glib2-2.58.1-1.h1.eulerosv2r8.aarch64.rpm
$ rpm -ivh --force libnghttp2-1.34.0-1.eulerosv2r8.aarch64.rpm
$ rpm -ivh --force libssh-0.8.3-3.h1.eulerosv2r8.aarch64.rpm
$ rpm -ivh --force brotli-1.0.5-1.eulerosv2r8.aarch64.rpm
```

安装yum之后还需要配置yum源为华为开源镜像站地址，可以参考 https://support.huaweicloud.com/csg_faq/csg_04_1209.html
登录Kit进入 /etc/yum.repos.d目录，修改euleros_aarch64.repo文件如下

```
[base]
name=EulerOS-2.0SP8 base
baseurl=http://mirrors.huaweicloud.com/euler/2.8/os/aarch64/
enabled=1
gpgcheck=1
gpgkey=http://mirrors.huaweicloud.com/euler/2.8/os/RPM-GPG-KEY-EulerOS
```
配置完成可以通过yum repolist查看

至此yum的安装就完成了，如果安装过程中缺少其他库可自行下载相应的rpm安装。
可以安装vim试一下，注意：安装的时候需要连接网络并关闭gpgcheck选项：

```
$ yum install vim --nogpgcheck
```