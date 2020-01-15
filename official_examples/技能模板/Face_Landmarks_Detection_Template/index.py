#! /usr/bin/python3.7
# 本模板展示多模型技能

import cv2
import hilens
from utils import *

def run():
    
    # 系统初始化，参数要与创建技能时填写的检验值保持一致
    hilens.init("landmarks")
    
    # 初始化自带摄像头与HDMI显示器
    camera  = hilens.VideoCapture()
    display = hilens.Display(hilens.HDMI)
    
    # 初始化模型：人脸检测模型(centerface)、人脸68个关键点检测模型
    centerface_model_path = hilens.get_model_dir() + "centerface_template_model.om"
    centerface_model      = hilens.Model(centerface_model_path)
    
    landmark_model_path   = hilens.get_model_dir() + "landmark68_template_model.om"
    landmark_model        = hilens.Model(landmark_model_path)
    
    # 本段代码展示如何录制HiLens Kit摄像头拍摄的视频
    fps    = 10
    size   = (1280, 720)
    format = cv2.VideoWriter_fourcc('M','J','P','G') # 注意视频格式
    writer = cv2.VideoWriter("face.avi", format, fps, size)
    
    # 待保存视频的起始帧数，可自行调节或加入更多逻辑
    frame_count = 0
    frame_start = 100
    frame_end   = 150
    uploaded    = False
    
    while True:
        # 读取一帧图片(YUV NV21格式)
        input_yuv = camera.read()
        
        # 图片预处理：转为RGB格式、缩放为模型输入尺寸
        img_rgb = cv2.cvtColor(input_yuv, cv2.COLOR_YUV2RGB_NV21)
        img_pre = preprocess(img_rgb)
    
        img_h, img_w = img_rgb.shape[:2]
        
        # 人脸检测模型推理，并进行后处理得到画面中最大的人脸检测框
        output   = centerface_model.infer([img_pre.flatten()])                
        face_box = get_largest_face_box(output, img_h, img_w)
        
        # 画面中检测到有人脸且满足一定条件
        if face_box is not None:
            # 截取出人脸区域并做预处理
            img_face  = preprocess_landmark(img_rgb, face_box)
            
            # 人脸关键点模型推理，得到68个人脸关键点
            output2   = landmark_model.infer([img_face.flatten()])
            landmarks = output2[0].reshape(68, 2)
            
            # 将人脸框和人脸关键点画在RGB图中
            img_rgb = draw_landmarks(img_rgb, face_box, landmarks)
        
        # 输出处理后的图像到HDMI显示器，必须先转换成YUV NV21格式
        output_nv21 = hilens.cvt_color(img_rgb, hilens.RGB2YUV_NV21)
        display.show(output_nv21)
        
        # 录制一段视频并发送到OBS中
        if not uploaded:
            frame_count += 1
            if frame_count > frame_end: # 录制结束点
                uploaded = True
                writer.release() # 先保存在本地
                ret = hilens.upload_file("face.avi", "face.avi", "write") # 发送到OBS中
                if ret != 0:
                    hilens.error("upload file failed!")
                    return
            elif frame_count > frame_start: # 录制开始点
                # 注意写入的图片格式必须为BGR
                writer.write(cv2.cvtColor(img_rgb, cv2.COLOR_RGB2BGR))
        
    hilens.terminate()

if __name__ == "__main__":
    run()