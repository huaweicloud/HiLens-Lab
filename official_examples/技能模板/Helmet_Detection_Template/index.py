#! /usr/bin/python3.7
import cv2
import time
import numpy as np
import threading

import hilens
from utils import *

# IPC摄像头处理线程
def camera_thread(camera_address, server_url):
    # 初始化IPC
    camera = hilens.VideoCapture(camera_address)
    
    # 初始化模型
    helmet_model_path = hilens.get_model_dir() + "helmet_template_model.om"
    helmet_model      = hilens.Model(helmet_model_path)
    
    while True:
        # 读取一帧图片(YUV NV21格式)
        input_yuv = camera.read()
        
        # 图片预处理：转为RGB格式、缩放为模型输入尺寸
        img_rgb = cv2.cvtColor(input_yuv, cv2.COLOR_YUV2RGB_NV21)        
        img_preprocess, img_w, img_h = preprocess(img_rgb)
    
        # 模型推理，并进行后处理得到检测框
        output = helmet_model.infer([img_preprocess.flatten()])        
        bboxes = get_result(output, img_w, img_h)
        
        # 从检测结果中判断是否有未佩戴安全帽的人
        if no_helmet(bboxes):
            # 后处理得到检测框，并在RGB图中画框
            img_rgb = draw_boxes(img_rgb, bboxes)
                
            # 以POST方式传输数据
            try:
                post_msg(server_url, img_rgb, bboxes)
            except Exception as e:
                hilens.error("post data failed!")
                print ("Reason : ", e)
            
            # 等待5秒后再检测，避免发送数据过多
            time.sleep(15)


def run():
    # 系统初始化，参数要与创建技能时填写的检验值保持一致
    hilens.init("helmet")    
   
    # 读取技能配置
    skill_cfg = hilens.get_skill_config()
    if skill_cfg is None or 'server_url' not in skill_cfg or 'IPC_address' not in skill_cfg:
        hilens.error("skill config not correct")
        return
    
    # 获取POST服务器地址和IPC地址，多个IPC地址用分号分隔开
    server_url  = skill_cfg['server_url']
    camera_list = skill_cfg['IPC_address'].split(';')
    
    # 每个IPC启动一个独立的线程
    threads_list = []
    for camera_address in camera_list:
        t = threading.Thread(target=camera_thread, args=(camera_address, server_url))
        t.start()
        threads_list.append(t)
    
    for t in threads_list:
        t.join()
        
    hilens.terminate()


if __name__ == "__main__":
    run()