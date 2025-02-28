#ifndef _MESSAGEQUEUE_H
#define _MESSAGEQUEUE_H

#include <iostream>
#include <string>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>

template <typename T>
class MessageQueue
{
private:
    std::queue<T> queue;
    std::mutex mtx;
    std::condition_variable cv;

public:
    void push(const T &msg);
    T pop();
    bool empty() const { return queue.empty(); }
    void clear();
};

template <typename T>
void MessageQueue<T>::push(const T &msg)
{
    std::lock_guard<std::mutex> lock(mtx);
    queue.push(msg);
    cv.notify_one();
}

template <typename T>
T MessageQueue<T>::pop()
{
    std::unique_lock<std::mutex> lock(mtx);
    cv.wait(lock, [this]
            { return !queue.empty(); });
    T msg = queue.front();
    queue.pop();
    return msg;
}


template <typename T>
void MessageQueue<T>::clear()
{
    while (!this->empty())
    {
        this->pop();
    }
}
#endif // !_MESSAGEQUEUE_H