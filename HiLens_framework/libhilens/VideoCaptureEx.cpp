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

#include "VideoCaptureEx.h"
#include <assert.h>
#include <unistd.h>
#include "sfw_log.h"

using namespace std;
using namespace hilens;

static const int SECOND_TIME = 1000000;

using Time = std::chrono::high_resolution_clock;
using us = std::chrono::duration<double, std::ratio<1, SECOND_TIME>>;
using elasptime = std::chrono::duration<double>;

void VideoCaptureEx::WaitAsync()
{
    for (std::thread &th : threadArray) {
        if (th.joinable()) {
            th.join();
        }
    }
    threadArray.clear();
    m_video_captures.clear();
}

void VideoCaptureEx::ThreadFunc()
{
    while (mbRunning.load()) {
        cbFunc(this);
    }
}

void VideoCaptureEx::CreateThread(int threadnum)
{
    if (threadnum < 0) {
        threadnum = 0;
    } else if (threadnum > MAX_ACCEPTTHREAD) {
        threadnum = MAX_ACCEPTTHREAD;
        ERROR("VideoCaptureEx::CreateThread: max thread num is %d", MAX_ACCEPTTHREAD);
    }
    mbRunning = true;
    long long starttime =
        std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch())
            .count();
    starttime -= mFpsTimeus;
    if (starttime < 0) {
        starttime = 0;
    }
    mStartTime = std::chrono::system_clock::time_point(std::chrono::microseconds(starttime));
    /* threadnum=0 就退化成类似同步模式调用了 */
    for (int i = 0; i < threadnum + 1; i++) {
        threadArray.push_back(std::thread(&VideoCaptureEx::ThreadFunc, this));
    }
}

VideoCaptureEx::ReadStatus VideoCaptureEx::ReadWithErrLocal(std::shared_ptr<hilens::VideoCapture> vcp, cv::Mat &frame)
{
    try {
        frame = vcp->Read();
        return READSTATUS_OK;
    } catch (const std::length_error &e) {
        if (strcmp(e.what(), "MP4Reader read finished!") == 0) {
            return READSTATUS_FILEEOF;
        } else {
            return READSTATUS_UNKNOW;
        }
    } catch (const std::runtime_error &e) {
        if (strcmp(e.what(), "MP4Reader read frame failed!") == 0) {
            return READSTATUS_FILEREADERROR;
        } else if (strcmp(e.what(), "MP4Reader frameData error!") == 0) {
            return READSTATUS_FILEDECERROR;
        } else if (strcmp(e.what(), "LocalCamera init failed!") == 0) {
            return READSTATUS_LOCALCAMBUSY;
        } else if (strcmp(e.what(), "RTSP Puller stopped!") == 0) {
            return READSTATUS_IPCSTOP;
        } else {
            return READSTATUS_UNKNOW;
        }
    }
}

void VideoCaptureEx::FpsDelay()
{
    if (mFpsTimeus > 0) {
        auto curTime = Time::now();
        elasptime fs = curTime - mStartTime;
        int needsleept = (int)(mFpsTimeus - std::chrono::duration_cast<us>(fs).count());
        if (needsleept > 0 && needsleept < SECOND_TIME) {
            usleep(needsleept);
        }
        mStartTime = Time::now();
    }
}

VideoCaptureEx::ReadStatus VideoCaptureEx::Read(cv::Mat &frame)
{
    std::lock_guard<std::mutex> lk(readmtx);
    FpsDelay();
    if (m_video_captures.size() > 0 && m_video_captures[0] != nullptr) {
        return ReadWithErrLocal(m_video_captures[0], frame);
    }
    return READSTATUS_BATCHALLSTOP;
}

VideoCaptureEx::ReadStatus VideoCaptureEx::BatchRead(std::vector<cv::Mat> &frames)
{
    std::lock_guard<std::mutex> lk(readmtx);
    FpsDelay();
    frames.clear();
    if (mCapturesCount <= 0) {
        return READSTATUS_BATCHALLSTOP;
    }
    int capturecount = 0;
    for (int i = 0; i < m_video_captures.size(); i++) {
        std::shared_ptr<hilens::VideoCapture> vcp;
        vcp = m_video_captures[i];
        cv::Mat readframe;
        ReadStatus ret = READSTATUS_UNKNOW;
        if (vcp) {
            ret = ReadWithErrLocal(vcp, readframe);
        }
        if (ret < 0) {
            LastErrStatus[i] = ret;
            m_video_captures[i] = nullptr;
        } else {
            LastErrStatus[i] = READSTATUS_OK;
            capturecount++;
        }
        frames.push_back(readframe);
    }
    mCapturesCount = capturecount;
    if (mCapturesCount <= 0) {
        return READSTATUS_BATCHALLSTOP;
    } else if (mCapturesCount == m_video_captures.size()) {
        return READSTATUS_OK;
    } else {
        return READSTATUS_BATCHSOMEERR;
    }
}

std::vector<VideoCaptureEx::ReadStatus> VideoCaptureEx::GetLastBatchErr()
{
    std::lock_guard<std::mutex> lk(readmtx);
    // 不要引用返回，避免被外面修改
    return LastErrStatus;
}

void VideoCaptureEx::ResetLastErr(size_t size)
{
    LastErrStatus.clear();
    LastErrStatus.resize(size);
}

bool VideoCaptureEx::BindAsyncInit(std::shared_ptr<hilens::VideoCapture> &vcp)
{
    if (vcp) {
        mStopReason = 0;
        mbRunning = false;
        WaitAsync();
        mbRunning = true;
        m_video_captures.push_back(vcp);
        mVideoWidth = m_video_captures[0]->Width();
        mVideoHeight = m_video_captures[0]->Height();
        vcp.reset();
        return true;
    } else {
        return false;
    }
}

bool VideoCaptureEx::BatchBindAsyncInit(std::vector<std::shared_ptr<hilens::VideoCapture>> &vcs)
{
    mCapturesCount = vcs.size();
    if (mCapturesCount > 0) {
        ResetLastErr(mCapturesCount);
        mStopReason = 0;
        mbRunning = false;
        WaitAsync();
        mbRunning = true;
        m_video_captures.swap(vcs);
        mVideoWidth = m_video_captures[0]->Width();
        mVideoHeight = m_video_captures[0]->Height();
        return true;
    } else {
        return false;
    }
}