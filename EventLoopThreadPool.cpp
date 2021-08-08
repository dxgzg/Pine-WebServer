#include "EventLoopThreadPool.h"
#include "EventLoopThread.h"
#include "EventLoop.h"


EventLoopThreadPool::EventLoopThreadPool(){
}
void EventLoopThreadPool::setThreadNum(int num){
    thread_num_ = num;
}
void EventLoopThreadPool::start(){
    for(int i = 0;i < thread_num_;++i){
        std::thread t(&EventLoopThreadPool::createEventLoop,this);
        threadPool_.emplace_back(std::move(t));
    }
}
EventLoop* EventLoopThreadPool::getNextLoop(){
    if(next_ >= eventLoopPtrPool_.size()){next_ = 0;}
    return eventLoopPtrPool_[next_++]->getLoop();
}
EventLoopThreadPool::~EventLoopThreadPool(){
    for(size_t i = 0;i < eventLoopPtrPool_.size();++i){
        threadPool_[i].detach(); // 先让他们放飞自我，后期会修改的
    }
}

void EventLoopThreadPool::createEventLoop(){
        EventLoopThreadPtr ptr = std::make_unique<EventLoopThread>();// 这样也能造一个loop出来
        EventLoop* loop = ptr->getLoop(); 
        eventLoopPtrPool_.emplace_back(std::move(ptr));
        loop->loop();
}