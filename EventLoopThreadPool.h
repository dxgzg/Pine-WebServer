#pragma once

#include <vector>
#include <memory>
#include <thread>
#include "EventLoopThread.h"
class EventLoopThread;
class EventLoop;

class EventLoopThreadPool
{
public:
    EventLoopThreadPool();
    EventLoop* getNextLoop();
    ~EventLoopThreadPool();
    // ~EventLoopThreadPool() __attribute__((noinline));
    void start();
    void setThreadNum(int num);
    void createEventLoop();
private:
    using EventLoopThreadPtr = std::unique_ptr<EventLoopThread>;
    std::vector<EventLoopThreadPtr> eventLoopPtrPool_;
    std::vector<std::thread> threadPool_;
    size_t next_ = 0;
    int thread_num_ = 2;
};
