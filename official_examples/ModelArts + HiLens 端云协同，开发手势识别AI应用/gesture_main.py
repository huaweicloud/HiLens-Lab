# -*- coding: utf-8 -*-
# !/usr/bin/python3
# SkillFramework 0.2.2 python demo —— 手势识别

import hilens
import cv2
import numpy as np

# labels = ["Great", "OK", "Rock", "Yeah"]

# labels = ["Great", "OK", "Rock", "Yeah","background","other "]
labels = ["Background", "Great", "OK", "Other","Rock","Yeah"]
score_thres = 0.6


def softmax(x):
    x = x-np.max(x,axis=0)
    ex = np.exp(x)
    return ex/np.sum(ex,axis=0)

def gesture_main():
    # 1. 模型初始化
    # model = hilens.Model("./model.om")
    model = hilens.Model(hilens.get_model_dir() + "model.om")
    display = hilens.Display(hilens.HDMI)
    camera = hilens.VideoCapture()
    
    hilens.set_log_level(hilens.INFO)
    hilens.info("Hand pose demo init successful!")    
	
    # 2. 手势识别
    while True:
        # 2.1 读取摄像头数据
        input_nv21 = camera.read() 
        input_rgb = cv2.cvtColor(input_nv21, cv2.COLOR_YUV2RGB_NV21)
        
        img_center = [640,360]
        left_top = [img_center[0]-240, img_center[1]-240]
        right_bot = [img_center[0]+240, img_center[1]+240]

        line_lengh = 40

        cv2.line(input_rgb,(left_top[0],left_top[1]),(left_top[0]+line_lengh,left_top[1]),(255,255,255),2)
        cv2.line(input_rgb,(left_top[0],left_top[1]),(left_top[0],left_top[1]+line_lengh),(255,255,255),2)
        cv2.line(input_rgb,(left_top[0],right_bot[1]),(left_top[0]+line_lengh,right_bot[1]),(255,255,255),2)
        cv2.line(input_rgb,(left_top[0],right_bot[1]),(left_top[0],right_bot[1]-line_lengh),(255,255,255),2)
        cv2.line(input_rgb,(right_bot[0],left_top[1]),(right_bot[0]-line_lengh,left_top[1]),(255,255,255),2)
        cv2.line(input_rgb,(right_bot[0],left_top[1]),(right_bot[0],left_top[1]+line_lengh),(255,255,255),2)
        cv2.line(input_rgb,(right_bot[0],right_bot[1]),(right_bot[0]-line_lengh,right_bot[1]),(255,255,255),2)
        cv2.line(input_rgb,(right_bot[0],right_bot[1]),(right_bot[0],right_bot[1]-line_lengh),(255,255,255),2)

        # 2.2 截取出一个正方形区域作为手势识别输入
        gesture_area = input_rgb[img_center[1]-240:img_center[1]+240, img_center[0]-240:img_center[0]+240, :]
        
        # 2.3 数据预处理
        input_resized = cv2.resize(gesture_area, (224, 224))        
        input_resized = np.transpose(input_resized,[2,0,1])
        input_resized = np.asarray(input_resized, dtype="float32") 

        # 2.3 模型推理
        outputs = model.infer([input_resized.flatten()])
        predict = softmax(outputs[0])
        max_inx = np.argmax(predict)
		
        thickness = 2
        #cv2.rectangle(input_rgb, (700, 160), (1100, 560), (255, 0, 0), thickness)
        # 2.4 结果展示 
        # 与
        if max_inx <= 5 and predict[max_inx] > score_thres:
            font = cv2.FONT_HERSHEY_SIMPLEX
            font_scale = 2
            
            if labels[max_inx] not in ["Background","Other"]:
                cv2.putText(input_rgb, labels[max_inx], (600, 100), font, font_scale, (255, 0, 0), thickness)

        output_nv21 = hilens.cvt_color(input_rgb, hilens.RGB2YUV_NV21)
        display.show(output_nv21) 

def run():
    ret = hilens.init("gesture") # 校验值在console创建技能是需要填写
    if ret != 0:
        hilens.error("Failed to initialize HiLens")
        return
        
    gesture_main()
    hilens.terminate()

if __name__ == '__main__':
    run()
