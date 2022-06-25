#include "EventLoopThreadPool.h"
#include "EventLoopThread.h"
#include "EventLoop.h"
#include "Logger.h"

#include <sstream>
#include <atomic>
using namespace std;
atomic<int> g_i;
EventLoopThreadPool::EventLoopThreadPool() = default;

void EventLoopThreadPool::setThreadNum(int num){
    LOG_INFO("set thread num:%d",num);
    this->thread_num_ = num;
}
void EventLoopThreadPool::start(){
    for(int i = 0;i < thread_num_;++i){
        std::thread t(&EventLoopThreadPool::createEventLoop,this);
        std::ostringstream oss;
        oss << t.get_id(); // 为了消除警告
//        LOG_INFO("create new %d thread threadId:%s",i + 1,oss.str().c_str());
        threadPool_.emplace_back(std::move(t));
    }
}
EventLoop* EventLoopThreadPool::getNextLoop(){
    if(next_ >= eventLoopPtrPool_.size()){next_ = 0;}
    LOG_INFO("线程个数:%lu",eventLoopPtrPool_.size());
    if(eventLoopPtrPool_[next_]->getLoop() == nullptr){
        LOG_FATAL("O NO线程池出错了");
    }
    return eventLoopPtrPool_[next_++]->getLoop();
}
EventLoopThreadPool::~EventLoopThreadPool(){
    for(size_t i = 0;i < eventLoopPtrPool_.size();++i){
        threadPool_[i].detach(); // todo 先让他们放飞自我，后期会修改的
    }
}

void EventLoopThreadPool::createEventLoop(){
    LOG_INFO("create new  thread threadId:%s",oss.str().c_str());
    EventLoopThreadPtr ptr = std::make_unique<EventLoopThread>();// 这样也能造一个loop出来
    EventLoop* loop = ptr->getLoop();
    std::unique_lock<std::mutex> lock(mutex_);
    eventLoopPtrPool_.emplace_back(std::move(ptr));
    // 不释放锁 raii会失效
    lock.unlock();
    loop->loop();
}