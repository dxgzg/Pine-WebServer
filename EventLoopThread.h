#pragma once

#include <memory>

class EventLoop;
// 这个类就是造个loop_出来的？
class EventLoopThread
{
public:
    EventLoopThread();
    //~EventLoopThread()__attribute__((noinline));
    ~EventLoopThread();
    EventLoop* getLoop()const{return loop_.get();}
private:
    std::unique_ptr<EventLoop> loop_;
};

