# -*- coding: utf-8 -*-
# !/usr/bin/python3
# SkillFramework 1.0.0 face detection demo
# yolo v2单类检测模型的后处理

import hilens
import cv2
import numpy as np
import math


# 网络输入尺寸
input_height = 480
input_width = 480

class_num = 1  # 类别数
box_num = 5  # 最后一层的anchor boxes数量
biases = [0.71, 0.70, 1.37, 1.31, 2.01, 2.21, 2.73, 2.81, 3.76, 3.9]  # 5个anchor box的宽高比例


def sigmoid(p):
    return 1.0 / (1 + math.exp(-p * 1.0))


def overlap(x1, w1, x2, w2):
    left = max(x1 - w1 / 2., x2 - w2 / 2.)
    right = min(x1 + w1 / 2., x2 + w2 / 2.)
    return right - left


def cal_iou(box, truth):
    w = overlap(box[0], box[2], truth[0], truth[2])
    h = overlap(box[1], box[3], truth[1], truth[3])
    if w <= 0 or h <= 0:
        return 0
    inter_area = w * h
    union_area = box[2] * box[3] + truth[2] * truth[3] - inter_area
    return inter_area / union_area


def apply_nms(boxes, threshold):
    """
    根据boxes的object置信度降序，应用NMS
    :param boxes: 待进行NMS的bounding boxes列表
    :param threshold: NMS阈值
    :return NMS后的bounding box列表
    """
    sorted_boxes = sorted(boxes, key=lambda d: d[4])[::-1]
    left_box_num = len(sorted_boxes)  # 当前剩余的box数量
    i = 0
    while i < left_box_num - 1:
        j = i + 1
        truth = sorted_boxes[i]
        while j < left_box_num:
            iou = cal_iou(sorted_boxes[j], truth)
            # iou大于阈值的移除掉，并且剩余box数减一
            if iou >= threshold:
                sorted_boxes.remove(sorted_boxes[j])
                left_box_num -= 1
            else:
                j += 1
        i += 1
    return sorted_boxes


def im_detect_nms(output_feature):
    """
    解析网络推理输出特征图，进行NMS得到最终人脸框
    :param output_feature: 网络输出的特征层numpy数组
    :return: 最终检测到的基于网络输入尺寸的人脸框列表
    """
    # 网络输出feature map的尺寸，（4坐标+1置信度+1类别）* 5 anchor, 15, 15
    res = np.reshape(output_feature, (30, 15, 15))
    c_, h_, w_ = res.shape
    num_ = box_num  # 每个grid预测5种anchor boxes
    # 1. 解析网络输出特征图，得到人脸候选框
    boxes = list()
    for h in range(h_):
        for w in range(w_):
            for n in range(num_):
                box = list()
                obj_score = sigmoid(res[n * 6 + 4, h, w])
                # 由于是单类，所以这里没必要考虑人脸类别的概率（归一化后都是1）
                if obj_score < 0.7:
                    continue
                
                x = (w + sigmoid(res[n * 6, h, w])) / float(w_)
                y = (h + sigmoid(res[n * 6 + 1, h, w])) / float(h_)
                ww = (math.exp(res[n * 6 + 2, h, w]) * biases[2 * n]) / float(w_)
                hh = (math.exp(res[n * 6 + 3, h, w]) * biases[2 * n + 1]) / float(h_)
                
                box.append(x)
                box.append(y)
                box.append(ww)
                box.append(hh)
                box.append(obj_score)
                boxes.append(box)
    # 2. 对候选框应用NMS
    res = apply_nms(boxes, 0.4)
    # 3. 将特征图人脸框按比例，调整回输入图像尺寸，得到最终检测到的人脸框
    box_list = []
    for box in res:
        left = (box[0] - box[2] / 2.0) * input_width
        right = (box[0] + box[2] / 2.0) * input_width
        top = (box[1] - box[3] / 2.0) * input_height
        bot = (box[1] + box[3] / 2.0) * input_height
        conf = box[4]
        box_list.append((left, top, right, bot, conf))
    
    return box_list
