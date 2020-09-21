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

#ifndef HILENS_BLOCKINGQUEUE_H
#define HILENS_BLOCKINGQUEUE_H

#include <queue>
#include <mutex>
#include <condition_variable>

namespace hilens {
template <class T> class BlockingQueue {
public:
    BlockingQueue() : maxc(1), shuttingdown(false) {}
    ~BlockingQueue();
    bool Push(const T &value);
    bool Pop(T &value);
    bool Full() const;
    bool Empty() const;
    bool WaitEmpty();
    bool WaitFull();
    void SetCapacity(int maxcap);
    void Clear();
    void ShutDown();
    bool IsShutDown();
    void Start();
    bool TryPop(T &value);
    bool TryPopNull();
    void NotifyAll();
    bool ForcePush(const T &value);
    int Size() const;

private:
    mutable std::mutex mtx;
    std::queue<T> que;
    std::condition_variable cond;
    int maxc;
    bool shuttingdown;
};

// --template class , the implement must the same place with .h, or the linker error
template <class T> BlockingQueue<T>::~BlockingQueue()
{
    ShutDown();
}

template <class T> void BlockingQueue<T>::Start()
{
    std::lock_guard<std::mutex> lk(mtx);
    shuttingdown = false;
}

template <class T> bool BlockingQueue<T>::IsShutDown()
{
    std::lock_guard<std::mutex> lk(mtx);
    return shuttingdown;
}

template <class T> void BlockingQueue<T>::ShutDown()
{
    std::lock_guard<std::mutex> lk(mtx);
    if (!shuttingdown) {
        shuttingdown = true;
        while (!que.empty()) {
            que.pop();
        }
        cond.notify_all();
    } else {
        cond.notify_all();
    }
}

template <class T> void BlockingQueue<T>::Clear()
{
    std::lock_guard<std::mutex> lk(mtx);
    if (!shuttingdown) {
        while (!que.empty()) {
            que.pop();
        }
        cond.notify_all();
    }
}

template <class T> bool BlockingQueue<T>::Push(const T &value)
{
    std::unique_lock<std::mutex> lk(mtx);
    if (!shuttingdown) {
        cond.wait(lk, [this] { return (shuttingdown || que.size() < maxc); });
        if (!shuttingdown) {
            que.push(value);
            cond.notify_one();
            return true;
        }
    }
    return false;
}

template <class T> bool BlockingQueue<T>::ForcePush(const T &value)
{
    std::unique_lock<std::mutex> lk(mtx);
    if (!shuttingdown) {
        que.push(value);
        cond.notify_one();
        return true;
    }
    return false;
}

template <class T> bool BlockingQueue<T>::Pop(T &value)
{
    std::unique_lock<std::mutex> lk(mtx);
    if (!shuttingdown) {
        cond.wait(lk, [this] { return (shuttingdown || !que.empty()); });
        if (!shuttingdown) {
            value = que.front();
            que.pop();
            cond.notify_one();
            return true;
        }
    }
    return false;
}

template <class T> bool BlockingQueue<T>::TryPop(T &value)
{
    std::lock_guard<std::mutex> lk(mtx);
    if (!shuttingdown && !que.empty()) {
        value = que.front();
        que.pop();
        cond.notify_one();
        return true;
    } else
        return false;
}

template <class T> bool BlockingQueue<T>::TryPopNull()
{
    std::lock_guard<std::mutex> lk(mtx);
    if (!shuttingdown && !que.empty()) {
        que.pop();
        cond.notify_one();
        return true;
    } else
        return false;
}

template <class T> void BlockingQueue<T>::NotifyAll()
{
    std::lock_guard<std::mutex> lk(mtx);
    if (!shuttingdown) {
        cond.notify_all();
    }
}

template <class T> bool BlockingQueue<T>::Full() const
{
    std::lock_guard<std::mutex> lk(mtx);
    return que.size() >= maxc;
}

template <class T> bool BlockingQueue<T>::WaitFull()
{
    std::unique_lock<std::mutex> lk(mtx);
    if (!shuttingdown) {
        bool willwait = !(shuttingdown || que.size() >= maxc);
        cond.wait(lk, [this] { return shuttingdown || que.size() >= maxc; });
        if (willwait) // 链式反应唤醒，如果这里消耗了一个， 要补回来
            cond.notify_one();
        return !shuttingdown;
    }
    return false;
}

template <class T> bool BlockingQueue<T>::Empty() const
{
    std::lock_guard<std::mutex> lk(mtx);
    return que.empty();
}

template <class T> bool BlockingQueue<T>::WaitEmpty()
{
    std::unique_lock<std::mutex> lk(mtx);
    if (!shuttingdown) {
        bool willwait = !(shuttingdown || que.empty());
        cond.wait(lk, [this] { return shuttingdown || que.empty(); });
        if (willwait) // 链式反应唤醒，如果这里消耗了一个， 要补回来
            cond.notify_one();
        return !shuttingdown;
    }
    return false;
}

template <class T> void BlockingQueue<T>::SetCapacity(int cap)
{
    std::lock_guard<std::mutex> lk(mtx);
    maxc = cap;
}

template <class T> int BlockingQueue<T>::Size() const
{
    std::lock_guard<std::mutex> lk(mtx);
    return que.size();
}
} // namespace UnifiedInfer
# endif // HILENS_BLOCKINGQUEUE_H