#include "Thread.h"
#include "EventLoop.h"

#include <functional>

Thread::Thread(EventLoop* loop):loop_(loop){
    std::thread  thread(&Thread::start,this);
}

void Thread::start(){
    loop_->loop();
}


