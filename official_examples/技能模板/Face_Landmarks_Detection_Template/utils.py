# -*- coding: utf-8 -*-
# !/usr/bin/python3
# utils for face landmarks detection
# 本技能中人脸检测使用的是centerface模型，相关原理请参考论文：https://arxiv.org/abs/1911.03599
# 人脸关键点检测使用的是VanillaCNN模型，相关原理请参考论文：https://arxiv.org/abs/1511.04031

import cv2
import numpy as np

# centerface检测模型输入尺寸
net_h = 288
net_w = 512

# 检测框的输出阈值与NMS筛选阈值
conf_threshold = 0.4
iou_threshold  = 0.4

# 画面中的人脸框大小阈值
face_size_threshold   = 100

# 嘴巴张合状态判断阈值
mouth_ratio_threshold = 0.4


# 人脸检测预处理：缩放到模型输入尺寸
def preprocess(img_data):
    new_image = cv2.resize(img_data, (net_w, net_h))
    return new_image

# 人脸关键点检测预处理：输入尺寸为40x40
def preprocess_landmark(img_data, face_box):
    img_face = img_data[face_box[1]:face_box[3], face_box[0]:face_box[2], :]
    
    face = cv2.cvtColor(img_face, cv2.COLOR_RGB2GRAY)
    face = cv2.resize(face, (40,40)).reshape(1,1,40,40)
    face = face.astype(np.float32)
    m = face.mean()
    s = face.std()
    face = (face - m) / s
    return face

# 从人脸检测模型输出中得到置信度最大的人脸框
def get_largest_face_box(model_outputs, img_h, img_w):

    stride = 4
    out_h  = int(net_h / stride)
    out_w  = int(net_w / stride)
    heatmap = model_outputs[0].reshape((1, out_h, out_w))
    scale   = model_outputs[1].reshape((2, out_h, out_w))
    offset  = model_outputs[2].reshape((2, out_h, out_w))

    scale_h   = net_h / img_h
    scale_w   = net_w / img_w

    heatmap          = heatmap[0, :, :]
    scale0, scale1   = scale[0, :, :], scale[1, :, :]
    offset0, offset1 = offset[0, :, :], offset[1, :, :]
    c0, c1           = np.where(heatmap > conf_threshold)
    
    if len(c0) > 0:
        boxes = [] # 候选人脸框
        largest_conf  = 0
        largest_index = 0
        for i in range(len(c0)):
            s0, s1 = np.exp(scale0[c0[i], c1[i]]) * 4, np.exp(scale1[c0[i], c1[i]]) * 4
            o0, o1 = offset0[c0[i], c1[i]], offset1[c0[i], c1[i]]
            s = heatmap[c0[i], c1[i]]
            x1, y1 = max(0, (c1[i] + o1 + 0.5) * 4 - s1 / 2), max(0, (c0[i] + o0 + 0.5) * 4 - s0 / 2)
            x1, y1 = min(x1, net_w), min(y1, net_h)
            
            # 记录置信度最大的人脸框下标
            if s > largest_conf:
                largest_conf  = s
                largest_index = i
            boxes.append([x1/scale_w, y1/scale_h, min(x1 + s1, net_w)/scale_w, min(y1 + s0, net_h)/scale_h])
        face_box = [int(x) for x in boxes[largest_index]]
        # 要求人脸框宽/高均超过100像素（可调节）
        if face_box[2] - face_box[0] > face_size_threshold and face_box[3] - face_box[1] > face_size_threshold:
            return face_box
    return None

# 根据嘴巴周围关键点求宽高比
def mouth_aspect_ratio(mouth):
    left_1_ver = np.linalg.norm(mouth[1]-mouth[11])
    left_2_ver = np.linalg.norm(mouth[2]-mouth[10])
    middle = np.linalg.norm(mouth[3]-mouth[9])
    right_1_ver = np.linalg.norm(mouth[4]-mouth[8])
    right_2_ver = np.linalg.norm(mouth[5]-mouth[7])
    horizontal = np.linalg.norm(mouth[0]-mouth[6])
    mar = (left_1_ver + left_2_ver + middle + right_1_ver + right_2_ver) / (5.0 * horizontal)
    return mar

# 画矩形框的四个角
def draw_square(img_data, left_top, right_bot, line_len, color):
    cv2.line(img_data, (left_top[0], left_top[1]),   (left_top[0]+line_len, left_top[1]),   color, 2)
    cv2.line(img_data, (left_top[0], left_top[1]),   (left_top[0], left_top[1]+line_len),   color, 2)
    cv2.line(img_data, (left_top[0], right_bot[1]),  (left_top[0]+line_len, right_bot[1]),  color, 2)
    cv2.line(img_data, (left_top[0], right_bot[1]),  (left_top[0], right_bot[1]-line_len),  color, 2)
    cv2.line(img_data, (right_bot[0], left_top[1]),  (right_bot[0]-line_len, left_top[1]),  color, 2)
    cv2.line(img_data, (right_bot[0], left_top[1]),  (right_bot[0], left_top[1]+line_len),  color, 2)
    cv2.line(img_data, (right_bot[0], right_bot[1]), (right_bot[0]-line_len, right_bot[1]), color, 2)
    cv2.line(img_data, (right_bot[0], right_bot[1]), (right_bot[0], right_bot[1]-line_len), color, 2)
    
# 在图中画出检测框，输出类别信息
def draw_landmarks(img_data, face_box, landmarks):

    # 画出人脸框的四个直角
    left_top  = (int(face_box[0]), int(face_box[1]))
    right_bot = (int(face_box[2]), int(face_box[3]))
    line_len  = 20
    color     = (0, 255, 0)
    draw_square(img_data, left_top, right_bot, line_len, color)

    # 画出68个人脸关键点
    width  = right_bot[0] - left_top[0]
    height = right_bot[1] - left_top[1]
    for x, y in landmarks:
        cv2.circle(img_data, (int(x*width + left_top[0]), int(y*height) + left_top[1]), 4, (255, 0, 0), -1)
    
    # 根据嘴巴周围关键点判断嘴巴的张合状态
    mouth = landmarks[48:68]
    mar   = mouth_aspect_ratio(mouth)
    if mar > mouth_ratio_threshold:
        text = 'mouth : open'
    else:
        text = 'mouth : close'
    
    # 在画面中打印嘴巴状态信息
    thickness  = 2
    font_scale = 2
    text_font  = cv2.FONT_HERSHEY_SIMPLEX
    cv2.putText(img_data, text, (450, 50), text_font, font_scale, (255, 0, 0), thickness)
            
    return img_data