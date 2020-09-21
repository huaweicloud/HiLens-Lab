/* *
 * Copyright 2020 Huawei Technologies Co., Ltd
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef LIBHILENS_VIDEOCAPTUREEX_H
#define LIBHILENS_VIDEOCAPTUREEX_H

#include <memory>
#include <string>
#include "video_capture.h"
#include <opencv2/opencv.hpp>
#include <functional>
#include <vector>
#include <thread>
#include <mutex>
#include <chrono>
#include <atomic>

#define MAX_ACCEPTTHREAD 5

namespace hilens {
/* *
 * @brief 视频采集器
 * 使用视频采集器来读取本地摄像头或IP摄像头的数据
 */
class VideoCaptureEx {
public:
    /* 函数指针定义，用来异步接收图像数据，一个摄像头会开2（或者配置）个线程，解码会等待5（或者配置）ms，不要采用Mat& */
    using VideoCaptureExCBFunc = std::function<void(VideoCaptureEx *)>;
    enum {
        ASYNC_NOTHREAD = 0,
        ASYNC_THREAD = 1
    };

    typedef enum {
        READSTATUS_BATCHSOMEERR = 1,   // batch操作有一些有问题，但是还可以继续
        READSTATUS_OK = 0,             // 读取正常
        READSTATUS_FILEEOF = -1,       // 读取本地文件到结尾
        READSTATUS_FILEREADERROR = -2, // 读取本地文件，读取出错
        READSTATUS_FILEDECERROR = -3,  // 读取本地文件，解码出错
        READSTATUS_LOCALCAMBUSY = -4,  // 内置摄像头被其它进程占用
        READSTATUS_IPCSTOP = -5,       // 远程网络摄像头异常停止
        READSTATUS_BATCHALLSTOP = -6,  // 所有的batch操作都已经停止了
        READSTATUS_UNKNOW = -100       // 未知错误
    } ReadStatus;

    /* *
     * @brief: 等待异步执行结束，可以用来阻止main函数的结束
     * @date: 2020-04-07
     */
    void WaitAsync();

    /* *
     * @brief: 停止当前的异步处理，并给出停止的原因
     * @param reason 停止的原因， 可以是ReadStatus， 也可以自定义
     * @date: 2020-04-08
     */
    inline void StopAsync(int reason)
    {
        mStopReason = reason;
        mbRunning = false;
    }

    /* *
     * @brief: 获得设置的异步停止的原因
     * @return 原因码
     * @date: 2020-04-08
     */
    inline int getStopReason()
    {
        return mStopReason;
    }

    /* *
     * @brief: 绑定回调函数
     * @param F&& f, Args&&... args： 模板可变参数，比如&callback 或者 &ClassName::callback, this
     * @date: 2020-04-07
     */
    template <class F, class... Args>
    inline void BindAsync(std::shared_ptr<hilens::VideoCapture> vcp, int threadnum, F &&f, Args &&... args)
    {
        if (BindAsyncInit(vcp)) {
            cbFunc = std::bind(std::forward<F>(f), std::forward<Args>(args)..., std::placeholders::_1);
            CreateThread(threadnum);
        }
    }

    /* *
     * @brief: 绑定回调函数
     * @param F&& f, Args&&... args： 模板可变参数，比如&callback 或者 &ClassName::callback, this
     * @date: 2020-04-07
     */
    template <class F, class... Args>
    inline void BatchBindAsync(std::vector<std::shared_ptr<hilens::VideoCapture> > &vcs, int threadnum, F &&f,
        Args &&... args)
    {
        if (BatchBindAsyncInit(vcs)) {
            cbFunc = std::bind(std::forward<F>(f), std::forward<Args>(args)..., std::placeholders::_1);
            CreateThread(threadnum);
        }
    }

    ReadStatus Read(cv::Mat &frames);
    ReadStatus BatchRead(std::vector<cv::Mat> &frames);
    std::vector<ReadStatus> GetLastBatchErr();
    inline int Width()
    {
        return mVideoWidth;
    }
    inline int Height()
    {
        return mVideoHeight;
    }
    std::mutex &GetMutex()
    {
        return protectmtx;
    }
    inline void SetFPS(int fps)
    {
        if (fps <= 0) {
            fps = 1;
        } else if (fps > 60) {
            fps = 60;
        }
        mFpsTimeus = 1000 * 1000 / fps;
    }

    VideoCaptureEx()
    {
        mFpsTimeus = 0; // no limit
        mStartTime = std::chrono::high_resolution_clock::now();
        mStopReason = 0;
        mCapturesCount = 0;
        threadArray.clear();
    }
    ~VideoCaptureEx()
    {
        mbRunning = false;
        WaitAsync();
    }

private:
    std::atomic_bool mbRunning;
    std::vector<std::thread> threadArray;
    VideoCaptureExCBFunc cbFunc;
    /* *
     * @brief: 启动2个线程，尽可能排满NPU的工作
     * @date: 2020-04-07
     */
    void CreateThread(int threadnum);
    void ThreadFunc();
    ReadStatus ReadWithErrLocal(std::shared_ptr<hilens::VideoCapture> vcp, cv::Mat &frame);
    void ResetLastErr(size_t size);
    bool BindAsyncInit(std::shared_ptr<hilens::VideoCapture> &vcp);
    bool BatchBindAsyncInit(std::vector<std::shared_ptr<hilens::VideoCapture> > &vcs);
    void FpsDelay();
    int mStopReason;
    std::vector<std::shared_ptr<hilens::VideoCapture> > m_video_captures;
    size_t mCapturesCount;
    std::mutex readmtx;
    std::vector<ReadStatus> LastErrStatus;
    int mVideoWidth;
    int mVideoHeight;
    int mFpsTimeus;
    std::chrono::system_clock::time_point mStartTime;
    std::mutex protectmtx;
};
} // namespace hilens
#endif // LIBHILENS_VIDEOCAPTUREEX_H