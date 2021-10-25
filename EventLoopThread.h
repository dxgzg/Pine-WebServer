#pragma once

#include <memory>
#include "Logger.h"

class EventLoop;
// 这个类就是造个loop_出来的？
class EventLoopThread
{
public:
    EventLoopThread();
    //~EventLoopThread()__attribute__((noinline));
    ~EventLoopThread();
    EventLoop* getLoop()const{
        if(loop_.get() == nullptr){
            LOG_FATAL("出错了");
        }
        return loop_.get();
    }
private:
    std::unique_ptr<EventLoop> loop_;
};

