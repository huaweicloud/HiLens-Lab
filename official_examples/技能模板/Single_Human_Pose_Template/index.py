#! /usr/bin/python3.7

import hilens
from utils import *

def run():
    # 系统初始化，参数要与创建技能时填写的检验值保持一致
    hilens.init("pose")
    
    # 初始化模型
    pose_model_path = hilens.get_model_dir() + "pose_template_model.om" 
    pose_model      = hilens.Model(pose_model_path)
    
    # 初始化USB摄像头与HDMI显示器
    camera       = hilens.VideoCapture()
    display_hdmi = hilens.Display(hilens.HDMI)
    
    while True:
        # 读取一帧图片(BGR格式)
        input_yuv = camera.read()

        # 图片预处理：转为BGR格式、裁剪/缩放为模型输入尺寸
        input_bgr      = cv2.cvtColor(input_yuv, cv2.COLOR_YUV2BGR_NV21)
        img_preprocess = preprocess(input_bgr)        
        
        # 模型推理
        model_outputs  = pose_model.infer([img_preprocess.flatten()])

        # 从推理结果中解码出人体关键点并画在图像中
        points   = get_points(input_bgr, model_outputs)
        img_data = draw_limbs(input_bgr, points)

        # 输出处理后的图像到HDMI显示器，必须先转换成YUV NV21格式
        output_nv21 = hilens.cvt_color(img_data, hilens.BGR2YUV_NV21)
        display_hdmi.show(output_nv21)
        
    hilens.terminate()


if __name__ == "__main__":
    run()