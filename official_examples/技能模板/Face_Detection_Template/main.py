# -*- coding: utf-8 -*-
# !/usr/bin/python3
# SkillFramework 1.0.0 face detection demo

import hilens
import cv2
import numpy as np
from postprocess import im_detect_nms

# 网络输入尺寸
input_height = 480
input_width = 480


def main():
    """  利用SkillFramework进行人脸检测模型的推理 """
    hilens.init("test") # 参数要与创建技能时填写的检验值保持一致
    model_path = hilens.get_model_dir() + "face_detection_demo.om" # 模型路径
    model = hilens.Model(model_path)
    display_hdmi = hilens.Display(hilens.HDMI)  # 图像通过hdmi输出到屏幕
    camera = hilens.VideoCapture()

    
    while True:
        # 1. 读取摄像头输入（yuv nv21）
        input_nv21 = camera.read()
        
        # 2. 转为bgr
        input_bgr = cv2.cvtColor(input_nv21, cv2.COLOR_YUV2BGR_NV21)
        src_image_height = input_bgr.shape[0]
        src_image_width = input_bgr.shape[1]
        
        # 3. 保留原图比例的resize为网络输入尺寸
        im_scale1 = float(input_width) / float(src_image_width)
        im_scale2 = float(input_height) / float(src_image_height)
        im_scale = min(im_scale1, im_scale2)
        input_bgr_rescaled = cv2.resize(input_bgr, None, None, fx=im_scale, fy=im_scale)
        input_bgr_resized = np.zeros((input_height, input_width, 3), dtype = np.uint8)
        input_bgr_resized[0:input_bgr_rescaled.shape[0],0:input_bgr_rescaled.shape[1],:] = input_bgr_rescaled
        
        # 3. 推理
        outputs = model.infer([input_bgr_resized.flatten()])
        
        # 4. 后处理得到人脸bounding box，恢复到原图比例，画人脸框
        detect_boxes = im_detect_nms(outputs[0])
        if len(detect_boxes) > 0:
            for rect in detect_boxes:
                left = max(rect[0] / im_scale, 0)
                top = max(rect[1] / im_scale, 0)
                right = min(rect[2] / im_scale, src_image_width)
                bottom = min(rect[3] / im_scale, src_image_height)
                cv2.rectangle(input_bgr, (int(left), int(top)), (int(right), int(bottom)), 255, 2)
        
        # 5. 输出图像，必须是yuv nv21形式
        output_nv21 = hilens.cvt_color(input_bgr, hilens.BGR2YUV_NV21)
        display_hdmi.show(output_nv21)


if __name__ == "__main__":
    
    main()
