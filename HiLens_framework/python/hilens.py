# This file is a python wrapper of hilensframework

import hilens_internal
import numpy as np
import json
import warnings
import functools


def deprecated(replacement):
    def deprecated(func):
        @functools.wraps(func)
        def new_func(*args, **kwargs):
            warnings.simplefilter('always', DeprecationWarning)  # turn off filter
            warnings.warn("Call to deprecated function {}, use {} for instead.".format(func.__name__, replacement),
                        category=DeprecationWarning,
                        stacklevel=2)
            warnings.simplefilter('default', DeprecationWarning)  # reset filter
            return func(*args, **kwargs)
        return new_func
    return deprecated

# 初始化时需要调用此接口来初始化
init = hilens_internal.Init

terminate = hilens_internal.Terminate

# 日志模块
TRACE = hilens_internal.TRACE
DEBUG = hilens_internal.DEBUG
INFO = hilens_internal.INFO
WARNING = hilens_internal.WARNING
ERROR = hilens_internal.ERROR
FATAL = hilens_internal.FATAL

trace = hilens_internal.Trace
debug = hilens_internal.Debug
info = hilens_internal.Info
warning = hilens_internal.Warning
error = hilens_internal.Error
fatal = hilens_internal.Fatal

set_log_level = hilens_internal.SetLogLevel

# 颜色转换模块
RGB2YUV_NV12 = hilens_internal.RGB2YUV_NV12
RGB2YUV_NV21 = hilens_internal.RGB2YUV_NV21
BGR2YUV_NV12 = hilens_internal.BGR2YUV_NV12
BGR2YUV_NV21 = hilens_internal.BGR2YUV_NV21

def cvt_color(src, code):
    '''
    转换图片的颜色格式。opencv原生未提供RGB/BGR到NV12/NV21的转换选项，故在这里做补充

    Params:
        src: 源图(BGR888或RGB888)
        code: 指定何种转换类型
    Returns:
        dst: 转换后的图片(NV12或NV21)
    '''
    rows, cols = src.shape[:2]
    dst = np.empty(rows * cols * 3 // 2, dtype=np.uint8)
    hilens_internal.CvtColorWrapper(src.flatten(), dst, rows, cols, code)
    return dst.reshape(rows * 3 // 2, cols)

# 定义一个异常类型，用于在HiLens的Model、VideoCapture等组件构造失败时抛出
class CreateError(Exception):
    def __init__(self, value):
        self.value = value
    def __str__(self):
        return repr(self.value)

class Preprocessor:
    '''
    硬件加速的预处理器
    '''
    def __init__(self):
        self.impl = hilens_internal.PreprocessorWrapper()
        if False == self.impl.Init():
            raise CreateError("failed to create Preprocessor")
    
    def resize(self, src, w, h, t):
        '''
        缩放图片

        Params:
            src: 源图，必须为NV21的格式。宽度范围[64, 1920], 2的倍数；高度范围[64, 1080], 2的倍数。
            w: 缩放宽度，范围[64, 1920], 2的倍数
            h: 缩放高度，范围[64, 1080], 2的倍数
            t: 目的图片的格式，0为NV21,1为NV12
        Returns:
            dst: 目的图片
        '''
        rows, cols = src.shape
        dst = np.empty(h * w * 3 // 2, dtype=np.uint8)
        if 0!= self.impl.ResizeArray(src.flatten(), dst, cols, rows, w, h, t):
            raise ValueError("failed to resize")
        return dst.reshape(h * 3 // 2, w)
    
    def crop(self, src, x, y, w, h, t):
        '''
        裁剪图片
        
        Params:
            src: 源图，必须为NV21的格式。宽度范围[64, 1920], 2的倍数；高度范围[64, 1080], 2的倍数。
            x: 裁剪区域左上角x坐标，范围[0, 1920], 2的倍数
            y: 裁剪区域左上角y坐标，范围[0, 1080], 2的倍数
            w: 裁剪宽度，范围[64, 1920], 2的倍数
            h: 裁剪高度，范围[64, 1080], 2的倍数
            t: 目的图片的格式，0为NV21,1为NV12
        Returns:
            dst: 目的图片
        '''
        rows, cols = src.shape
        dst = np.empty(h * w * 3 // 2, dtype=np.uint8)
        if 0!= self.impl.CropArray(src.flatten(), dst, cols, rows, x, y, w, h, t):
            raise ValueError("failed to crop")
        return dst.reshape(h * 3 // 2, w)

class Model:
    '''
    模型管理器。使用模型管理器加载模型并进行推理
    '''
    def __init__(self, filename):
        '''
        构造函数

        Params:
            filename: 模型文件的绝对路径
        '''
        self.impl = hilens_internal.ModelWrapper()
        if False == self.impl.Init(filename):
            raise CreateError("failed to create Model")
    
    def infer(self, inputs):
        '''
        推理

        Params:
            inputs: 推理输入，一组uint8数组组成的list
        Returns:
            outputs: 推理输出，一组float数组组成的list
        '''
        input_vec = hilens_internal.InferDataVec()
        output_vec = hilens_internal.InferDataVec()
        for i in inputs:
            if i.dtype=='uint8':
                input_vec.push_back(hilens_internal.InferDataWrapper(i))
            elif i.dtype=='float32':
                input_vec.push_back(hilens_internal.InferDataWrapper(i, 0))
            else:
                raise ValueError("input dtype not supported! (valid dtype: uint8, float32)")
        ret = self.impl.InferWrapper(input_vec, output_vec)
        if ret != 0:
            return ret
        outputs = []
        for o in output_vec:
            # // 4是因为o.size是按字节来算的，而float类型是4字节
            outputs.append(o.ToArrayFloat(o.size // 4))
        return outputs

HDMI = hilens_internal.Display.HDMI
RTMP = hilens_internal.Display.RTMP
H264_FILE = hilens_internal.Display.H264_FILE

class Display:
    '''
    显示器。用来将图片显示到HDMI接口的显示器或者HiLens Console上
    '''
    def __init__(self, t, path=None):
        '''
        构造函数

        Params:
            t: 类型，可选HDMI、RTMP或H264_FILE
        '''
        self.impl = hilens_internal.DisplayWrapper()
        if path is None:
            if False == self.impl.Init(t):
                raise CreateError("failed to create Display")
        else:
            if False == self.impl.Init(t, path):
                raise CreateError("failed to create Display")

    def show(self, frame):
        '''
        显示一张图片。在第一次调用该方法时，Display会根据输入的图片尺寸来设置视频尺寸，
        此后的调用中skill必须保证输入图片的尺寸与之前的一致。

        Params:
            frame: 要显示的图片，必须为NV21格式
        Returns:
            ret: 0为成功，其他为失败
        '''
        rows, cols = frame.shape
        return self.impl.ShowArray(frame.flatten(), cols, rows)

class VideoCapture:
    '''
    视频采集器。使用视频采集器来读取本地摄像头或IP摄像头的数据
    '''
    def __init__(self, *args):
        '''
        构造一个视频采集器。如果不填参数则打开本地摄像头。如果填写参数则打开设备配置中对应的IPC摄像头

        Params:
            （可选）name: 设备配置中IPC摄像头的名字
        '''
        self.is_uvc = False
        self.destWidth = 0
        self.destHeight = 0

        if len(args) == 0:
            self.impl = hilens_internal.VideoCaptureWrapper()
            if not self.impl.Init():
                raise CreateError("failed to create VideoCapture")
        elif len(args) == 1:
            self.impl = hilens_internal.VideoCaptureWrapper()
            if int == type(args[0]):
                self.is_uvc = True
            if not self.impl.Init(args[0]):
                raise CreateError("failed to create VideoCapture")
        elif len(args) == 3:
            self.impl = hilens_internal.VideoCaptureWrapper()
            if int == type(args[0]):
                self.is_uvc = True
            if not self.impl.Init(args[0], args[1], args[2]):
                raise CreateError("failed to create VideoCapture")
            else:
                self.destWidth = args[1]
                self.destHeight = args[2]
        else:
            raise ValueError("incorrect parameter")

        self.width = self.impl.Width()
        self.height = self.impl.Height()

        #如果没有指定，默认输出的尺寸为原始尺寸
        if self.destHeight == 0:
            self.destHeight = self.height
        if self.destWidth == 0:
            self.destWidth = self.width

    def read(self):
        '''
        读取一帧视频。
        '''
        frame = None
        if self.is_uvc:
            frame = self.impl.ReadArray(self.destWidth * self.destHeight * 3).reshape(self.destHeight, self.destWidth, 3) # UVC类型的摄像头返回的是BGR颜色排布的数据
        else:
            frame = self.impl.ReadArray(self.destWidth * self.destHeight * 3 // 2).reshape(self.destHeight * 3 // 2, self.destWidth) # IPC摄像头返回的是YUV_NV21颜色排布的数据
        if self.impl.ReadError():
            raise RuntimeError("Failed to read frame")
        return frame

# 上传文件到OBS
@deprecated("upload_file_to_obs")
def upload_file(key, filepath, mode):
    return hilens_internal.UploadFile(key, filepath, mode)

def upload_file_to_obs(key, filepath, mode):
    return hilens_internal.UploadFile(key, filepath, mode)

# 上传buffer到OBS
@deprecated("upload_buffer_to_obs")
def upload_bufer(key, buffer, mode):
    if isinstance(buffer, str):
        buffer = buffer.encode("UTF-8")
    npbuffer = np.frombuffer(buffer, dtype=np.uint8)
    return hilens_internal.UploadBuffer(key, npbuffer, mode)

def upload_buffer_to_obs(key, buffer, mode):
    if isinstance(buffer, str):
        buffer = buffer.encode("UTF-8")
    npbuffer = np.frombuffer(buffer, dtype=np.uint8)
    return hilens_internal.UploadBuffer(key, npbuffer, mode)

# 发送消息
def send_msg(subject, message):
    return hilens_internal.SendMessage(subject, message)

# 返回技能工作区目录的路径（末尾带"/"）
# 不允许在技能安装目录下写操作，故需要指定各技能可写的工作区位置
get_workspace_path = hilens_internal.GetWorkspacePath

# 返回技能模型所在目录（末尾带"/"）
get_model_dir = hilens_internal.GetModelDirPath

# 获取技能配置
def get_skill_config():
    return json.loads(hilens_internal.GetSkillConfigText())

# 从OBS下载文件
# 第一个参数是OBS链接，第二个参数指定下载到哪个目录
download_from_obs = hilens_internal.DownloadFileFromOBS

# 计算文件的md5值
md5_of_file = hilens_internal.MD5ofFile

# 调用EI_Services服务
# 调用方法
GET = hilens_internal.EI_GET
POST = hilens_internal.EI_POST
PUT = hilens_internal.EI_PUT
DELETE = hilens_internal.EI_DELETE
# 发送请求头，添加：EIHeaders.push_back("Content-Type: application/json")
EIHeaders = hilens_internal.EIHeaders
EIHeaders.add = EIHeaders.push_back
# 请求响应，EIResponse.responseCode=0表示请求发送成功，EIResponse.responseBody为请求响应体
EIResponse = hilens_internal.EIResponse
"""
调用华为云EI_Services服务
目前支持：
1.发送通用请求，是否需要开通请咨询相关服务
Request(method, host, uri, queryParams, payload, headers)
2.调用hilens服务，HumanDetect和LicensePlate请联系hilens工作人员开通后使用
HumanDetect(image_base64)
FaceAttribute(image_base64)
LicensePlate(image_base64)
DogShitDetect(image_base64)
3.调用Face服务，需要开通Face服务相关接口
SearchFace(face_set_name, image_base64, top_n, threshold, filter)
AddFace(face_set_name, image_base64, external_image_id)
FaceDetect(image_base64, attributes)
FaceCompare(image1_base64, image2_base64)
LiveDetect(video_base64, actions, actiontime)
"""
EIServices = hilens_internal.EIServices

class AudioCapture:
    '''
    音频采集器，使用音频采集器来读取音频数据
    '''
    def __init__(self, file_path=None):
        '''
        构造一个音频采集器。打开本地相应目录的音频文件或者从麦克风读取
        Params:
            file_path: 如果参数为空，从麦克风读取，如果是音频文件路径，从文件读取
        '''
        self.impl = hilens_internal.AudioCaptureWrapper()
        
        if str == type(file_path):
            if False == self.impl.Init(file_path):
                raise CreateError("Failed to create AudioCapture")
        else:
            if False == self.impl.Init():
                raise CreateError("Failed to create AudioCapture")

    def read(self, nFrames=1):
        '''
        读取nFrames帧音频。
        '''
        if 0 != self.impl.ReadArray(nFrames):
            raise RuntimeError("Failed to read frames")
            
        data = self.impl.ToNumpyArray(self.impl.totalSize).reshape(nFrames, -1)
        data.dtype = np.short
        return data
        