# !/usr/sbin/bash

BASE_PATH=$(cd `dirname $0`;pwd)
INSTALL_PATH="/home/hilens/skillframework/"
LOG_PATH="/home/log/alog/hilens/skillframework/"
PACKAGE_NAME="HiLensFramework.tar.gz"

ASSERTS=(bin configs lib python scripts VERSION)

echo "hilensframework Installation"

activate(){
    echo "activating..."
    # 添加lib目录到索引
    echo "${INSTALL_PATH}/lib" > /etc/ld.so.conf.d/hilens.conf
    ldconfig

    # 链接python
    ln -sf /home/hilens/skillframework/python/hilens.py /usr/lib64/python3.7/site-packages/hilens.py
    ln -sf /home/hilens/skillframework/python/hilens_internal.py /usr/lib64/python3.7/site-packages/hilens_internal.py
    ln -sf /home/hilens/skillframework/lib/_hilens_internal.so /usr/lib64/python3.7/site-packages/_hilens_internal.so
    ln -sf /home/hilens/skillframework/bin/deviceHealthCheck /usr/bin/health_check

    # 禁用rpath后需要放到系统目录
    cp -rf /home/hilens/skillframework/lib/libnanomsg* /usr/lib64/
    cp -rf /home/hilens/skillframework/lib/liblog4cplus* /usr/lib64/

    # 只有在有mpp的时候才启动hilensd
    if [ -c "/dev/vpss" -a -c "/dev/vdec" -a -c "/dev/venc" ]; then
        # 创建服务
        echo "[Unit]" > /lib/systemd/system/hilensd.service
        echo "Description=This is hilens daemon service" >> /lib/systemd/system/hilensd.service
        echo "[Service]" >> /lib/systemd/system/hilensd.service
        echo "Environment=PAAS_CRYPTO_PATH=/opt/material/" >> /lib/systemd/system/hilensd.service
        echo "Type=simple" >> /lib/systemd/system/hilensd.service
        echo "Restart=on-failure" >> /lib/systemd/system/hilensd.service
        echo "RestartSec=3s" >> /lib/systemd/system/hilensd.service      
        echo "ExecStart=${INSTALL_PATH}bin/hilensd" >> /lib/systemd/system/hilensd.service
        echo "ExecStop=" >> /lib/systemd/system/hilensd.service
        echo "[Install]" >> /lib/systemd/system/hilensd.service
        echo "WantedBy=multi-user.target" >> /lib/systemd/system/hilensd.service
        echo "Alias=hilens" >> /lib/systemd/system/hilensd.service
	
	echo "[Unit]" > /lib/systemd/system/hilensdAudio.service
        echo "Description=This is hilens daemon service" >> /lib/systemd/system/hilensdAudio.service
        echo "[Service]" >> /lib/systemd/system/hilensdAudio.service
        echo "Environment=PAAS_CRYPTO_PATH=/opt/material/" >> /lib/systemd/system/hilensdAudio.service
        echo "Type=simple" >> /lib/systemd/system/hilensdAudio.service    
	echo "Restart=on-failure" >> /lib/systemd/system/hilensdAudio.service
        echo "RestartSec=3s" >> /lib/systemd/system/hilensdAudio.service  
        echo "ExecStart=${INSTALL_PATH}bin/hilensdAudio" >> /lib/systemd/system/hilensdAudio.service
        echo "ExecStop=" >> /lib/systemd/system/hilensdAudio.service
        echo "[Install]" >> /lib/systemd/system/hilensdAudio.service
        echo "WantedBy=multi-user.target" >> /lib/systemd/system/hilensdAudio.service
        echo "Alias=hilens" >> /lib/systemd/system/hilensdAudio.service
        # 启动服务
        systemctl daemon-reload
        systemctl restart hilensd.service
	systemctl restart hilensdAudio.service
    fi
}

install(){
    echo "installing..."

    #拷贝旧的日志到新的目录并删除旧的目录
    mkdir -p ${LOG_PATH}
    if [ -d ${INSTALL_PATH}/log ];then
        cp -R ${INSTALL_PATH}/log/* ${LOG_PATH}
        rm -rf ${INSTALL_PATH}/log
    fi
    chmod 755 ${LOG_PATH}
    chown root:root -R ${LOG_PATH}

    # 先删掉旧的数据
    if [ -d ${INSTALL_PATH}/lib ];then
        rm -rf ${INSTALL_PATH}/lib
    fi
    mkdir -p ${INSTALL_PATH}/lib
    chmod 755 ${INSTALL_PATH}/lib

    tar -C ${INSTALL_PATH} -xf ${BASE_PATH}/${PACKAGE_NAME}
    chown root:root -R ${INSTALL_PATH}
    
    activate

    echo "finished"
    return 0
}

if [[ $1 == "activate" ]]
then
    if [[ ! -d ${INSTALL_PATH} ]];then
        echo "Not exist, installing..."
        install
    else
        activate
    fi
else
    install
fi
