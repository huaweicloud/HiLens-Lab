# -*- coding: utf-8 -*-
# !/usr/bin/python3
# utils for human joints detection
#
# 相关原理可查阅openpose论文：https://arxiv.org/abs/1812.08008
#

import cv2
import math
import numpy as np

# 人体关键点检测模型输入尺寸
net_h = 288
net_w = 288

# 关键点判断与关节判断的阈值
threshold_1 = 0.3
threshold_2 = 0.2

# 关节连接的关键点序号
limbSeq = [[1, 0], [1, 2], [1, 5], [2, 3], [3, 4], [5, 6], [6, 7], [1, 8], [8, 9],
           [9, 10], [1, 11], [11, 12], [12, 13], [0, 14], [14, 16], [0, 15], [15, 17]]

# 关节的特征矩阵对应信息
mapIdx = [[28, 29], [12, 13], [20, 21], [14, 15], [16, 17], [22, 23], [24, 25], [0, 1],
          [2, 3], [4, 5], [6, 7], [8, 9], [10, 11], [30, 31], [34, 35], [32, 33], [36, 37]]

# 预先定义的颜色值，用于画关节
colors = [[255, 0, 0], [255, 85, 0], [255, 170, 0], [255, 255, 0], [170, 255, 0], [85, 255, 0], 
          [0, 255, 0], [0, 255, 85], [0, 255, 170], [0, 255, 255], [0, 170, 255], [0, 85, 255], 
          [0, 0, 255], [85, 0, 255], [170, 0, 255], [255, 0, 255], [255, 0, 170], [255, 0, 85]]

# 图片预处理：裁剪出画面中间区域，缩放到模型输入尺寸
def preprocess(img_data):    
    h, w, c   = img_data.shape # default: 1280*720
    shift     = int((w-h)/2)
    img_data2 = img_data[:, shift:shift+h, :]
    new_image = cv2.resize(img_data2, (net_w, net_h))

    return new_image

# 从一个关键点特征矩阵中解码出候选关键点（超过阈值的局部极大值点）
def find_peaks(heatmap):
    map_left = np.zeros(heatmap.shape)
    map_left[1:, :] = heatmap[:-1, :]
    map_right = np.zeros(heatmap.shape)
    map_right[:-1, :] = heatmap[1:, :]
    map_up = np.zeros(heatmap.shape)
    map_up[:, 1:] = heatmap[:, :-1]
    map_down = np.zeros(heatmap.shape)
    map_down[:, :-1] = heatmap[:, 1:]

    peaks_binary = np.logical_and.reduce((heatmap >= map_left, 
                                          heatmap >= map_right, 
                                          heatmap >= map_up, 
                                          heatmap >= map_down, 
                                          heatmap > threshold_1))
    peaks = list(zip(np.nonzero(peaks_binary)[1], np.nonzero(peaks_binary)[0]))
    return peaks
    
# 从一个关节特征矩阵中解码出候选关节    
def find_limbs(pafmap, index, candidate_peaks, height):    
    mid_num   = 5
    score_mid = pafmap[:, :, mapIdx[index]]
    candA     = candidate_peaks[limbSeq[index][0]]
    candB     = candidate_peaks[limbSeq[index][1]]
    
    candidate_limbs = []
    for i in range(len(candA)):
        for j in range(len(candB)):
            vec  = np.subtract(candB[j], candA[i])
            norm = math.sqrt(vec[0] * vec[0] + vec[1] * vec[1])
            if norm == 0:
                continue
            vec = np.divide(vec, norm)

            startend = list(zip(np.linspace(candA[i][0], candB[j][0], num=mid_num), \
                                np.linspace(candA[i][1], candB[j][1], num=mid_num)))

            vec_x = np.array([score_mid[int(round(startend[I][1])), int(round(startend[I][0])), 0] \
                            for I in range(len(startend))])
            vec_y = np.array([score_mid[int(round(startend[I][1])), int(round(startend[I][0])), 1] \
                            for I in range(len(startend))])

            score_mids = np.multiply(vec_x, vec[0]) + np.multiply(vec_y, vec[1])
            score_norm = sum(score_mids) / len(score_mids) + min(0.5 * height / norm - 1, 0)
            criterion1 = len(np.nonzero(score_mids > threshold_2)[0]) > 0.8 * len(score_mids)
            criterion2 = score_norm > 0
            if criterion1 and criterion2:
                candidate_limbs.append([i, j, score_norm])
    return candidate_limbs

# 从推理结果中解码出人体关键点
def get_points(img_original, model_outputs):   
    h, w, c = img_original.shape
    
    # 模型推理结果有两个，model_outputs[1]对应关键点特征矩阵
    num_channel = 19
    heatmap = model_outputs[1].reshape((num_channel, net_h // 8, net_w // 8)).transpose((1,2,0))
    heatmap = cv2.resize(heatmap, (h // 8, h // 8), interpolation=cv2.INTER_CUBIC)
    
    # model_outputs[0]对应关节信息特征矩阵
    pafmap = model_outputs[0].reshape((num_channel*2, net_h // 8, net_w // 8)).transpose((1,2,0))
    pafmap = cv2.resize(pafmap, (h // 8, h // 8), interpolation=cv2.INTER_CUBIC)

    # 找出所有的候选关键点
    candidate_peaks = []
    for part in range(18):
        peaks = find_peaks(heatmap[:, :, part])
        candidate_peaks.append(peaks)

    # 根据关节确定关键点
    points = [(-1, -1) for ix in range(18)]
    for k in range(len(mapIdx)):
        candidate_limbs = find_limbs(pafmap, k, candidate_peaks, img_original.shape[0] / 8)
        if len(candidate_limbs) > 0:
            max_cand  = max(candidate_limbs, key=lambda x: x[2])
            max_peakA = candidate_peaks[limbSeq[k][0]][max_cand[0]]
            candidate_peaks[limbSeq[k][0]] = [candidate_peaks[limbSeq[k][0]][max_cand[0]]]
            candidate_peaks[limbSeq[k][1]] = [candidate_peaks[limbSeq[k][1]][max_cand[1]]]
            
            points[limbSeq[k][0]] = (candidate_peaks[limbSeq[k][0]][0][0], candidate_peaks[limbSeq[k][0]][0][1])
            points[limbSeq[k][1]] = (candidate_peaks[limbSeq[k][1]][0][0], candidate_peaks[limbSeq[k][1]][0][1])

    # 将关键点坐标还原到原图中间位置
    scale  = 8
    shift  = int((w-h)/2)
    points = [(int(point[0] * scale) + shift, int(point[1] * scale)) for point in points]

    return points

# 计算两个向量的夹角    
def get_cross_angle(point1_1, point1_2, point2_1, point2_2):
    if point1_1[0] < 0 or point1_2[0] < 0 or point2_1[0] < 0 or point2_2[0] < 0:
        return -1.0;
        
    arr_0 = np.array([(point1_2[0] - point1_1[0]), (point1_2[1] - point1_1[1])])
    arr_1 = np.array([(point2_2[0] - point2_1[0]), (point2_2[1] - point2_1[1])])
    cos_val = (float(arr_0.dot(arr_1)) / (np.sqrt(arr_0.dot(arr_0)) * np.sqrt(arr_1.dot(arr_1))))
    angle = np.arccos(cos_val) * (180 / np.pi)
    if not (angle >= 0 and angle <= 360):
        angle = -1.0
    return angle

# 计算左右大臂、小臂与垂直方向的夹角
def get_arm_angles(points):
    # 右臂
    r_shoulder = 2
    r_elbow    = 3
    r_wrist    = 4
    angle1     = get_cross_angle(points[r_shoulder], points[r_elbow], points[r_shoulder], (points[r_shoulder][0], points[r_shoulder][1] + 10))
    angle2     = get_cross_angle(points[r_elbow], points[r_wrist], points[r_elbow], (points[r_elbow][0], points[r_elbow][1] + 10))

    # 左臂
    l_shoulder = 5
    l_elbow    = 6
    l_wrist    = 7
    angle3     = get_cross_angle(points[l_shoulder], points[l_elbow], points[l_shoulder], (points[l_shoulder][0], points[l_shoulder][1] + 10))
    angle4     = get_cross_angle(points[l_elbow], points[l_wrist], points[l_elbow], (points[l_elbow][0], points[l_elbow][1] + 10))
    
    return angle1, angle2, angle3, angle4

# 根据左右大臂、小臂与垂直方向的夹角判断是否为比心动作，若是屏幕中打印红色'L~O~V~E'字样
def draw_heart(img_data, points):
    angle1, angle2, angle3, angle4 = get_arm_angles(points)
    if angle1 > 100 and angle1 < 160 and angle2 > 100 and angle2 < 160 and \
       angle3 > 100 and angle3 < 160 and angle4 > 100 and angle4 < 160:
        text_font  = cv2.FONT_HERSHEY_SIMPLEX
        cv2.putText(img_data, 'L~O~V~E', (500, 60), text_font, 2, (0,0,255), 5)

# 在图像中画出关节
def draw_limbs(img_original, points):
    width  = 4
    canvas = img_original # B,G,R order
   
    h, w, c = canvas.shape # default: 1280*720
    shift   = int((w-h)/2)
    
    # 左右两侧画成灰色
    canvas[:, :shift, :]   = 128
    canvas[:, h+shift:, :] = 128
    
    # 用细长的实心椭圆画出关节
    for line in limbSeq:
        X = points[line[0]]
        Y = points[line[1]]
        if X[0] < 0 or X[1] < 0 or Y[0] < 0 or Y[1] < 0:
            continue
        
        center = (int((X[0]+Y[0])/2), int((X[1]+Y[1])/2))
        length = ((X[0] - Y[0]) ** 2 + (X[1] - Y[1]) ** 2) ** 0.5
        angle = math.degrees(math.atan2(X[1] - Y[1], X[0] - Y[0]))
        polygon = cv2.ellipse2Poly((center[0], center[1]), (int(length / 2), width), 
                    int(angle), 0, 360, 1)
        cv2.fillConvexPoly(canvas, polygon, colors[line[1]])
    
    # 识别比心动作并在屏幕中显示
    draw_heart(canvas, points)
    
    return canvas
    