#include "HeadDetection.h"
#include "Channel.h"
#include "EventLoop.h"

#include <sys/timerfd.h>
#include <assert.h>
#include <sys/epoll.h>


static int creatTimeFd(){
    return timerfd_create(CLOCK_REALTIME, TFD_NONBLOCK);
}
static __time_t getNow(){
    timespec now;
    int ret = clock_gettime(CLOCK_REALTIME, &now);//获取时钟时间
    assert(ret != -1);
    return now.tv_sec;
}

HeadDetection::HeadDetection(EventLoop* loop,int expire):
    NodeList_(),NodeMap_(),fdMap_(),timefd_(creatTimeFd()),
    expire_(expire),loop_(loop),channel_(std::make_unique<Channel>(loop_,timefd_))    
{
    assert(timefd_ != -1);
    
    channel_->setReadCallback(std::bind(&HeadDetection::headRead,this));
    channel_->enableReadEvent();
    itimerspec new_value;
    new_value.it_value.tv_sec = expire_;
    new_value.it_value.tv_nsec = 0;

    new_value.it_interval.tv_sec = expire_;
    new_value.it_interval.tv_nsec = 0;
    
    int ret = timerfd_settime(timefd_, 0, &new_value, NULL);//启动定时器
    assert(ret != -1);
}

void HeadDetection::add(int fd,std::function<void()> callback){
    if(fdMap_.find(fd) == fdMap_.end()){
        NodePtr ptr = std::make_shared<Node>();
        ptr->fd_ = fd;
        ptr->time_ = getNow();
        ptr->callback_ = std::move(callback);
        fdMap_.emplace(fd,ptr);
        NodeList_.emplace_front(ptr);
        NodeMap_.emplace(ptr,NodeList_.begin());
    }
}

void HeadDetection::adjust(int fd){
    auto ptr = fdMap_[fd];
    ptr->time_ = getNow();
    auto it = NodeMap_[ptr];
    NodeList_.erase(it);
    NodeList_.emplace_front(ptr);
    NodeMap_[ptr] = NodeList_.begin();
}

void HeadDetection::headRead(){
    struct timespec now;
    int ret = clock_gettime(CLOCK_REALTIME, &now);//获取时钟时间
    assert(ret != -1);
    while(!NodeList_.empty()){
        auto ptr = *NodeList_.rbegin();
        __time_t time = ptr->time_ + expire_;
        if(time < now.tv_sec){
            del(ptr);
        }
        else{
            break;
        }
    }
}


void HeadDetection::del(NodePtr ptr){
    ptr->callback_();
    auto it = NodeMap_[ptr];
    NodeList_.erase(it);
    fdMap_.erase(ptr->fd_);
    NodeMap_.erase(ptr);
}