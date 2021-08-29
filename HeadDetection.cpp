#include "HeadDetection.h"
#include "Channel.h"
#include "EventLoop.h"
#include "HeadDetection.h"
#include "Logger.h"

#include <sys/timerfd.h>
#include <assert.h>
#include <sys/epoll.h>

static bool isSameThread(const std::thread::id& thread_id){
    return thread_id == std::this_thread::get_id();
}

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

void HeadDetection::add(int fd,std::shared_ptr<TcpClient> cPtr,std::function<void()> callback){
    assert(isSameThread(loop_->getThreadId()));
    if(fdMap_.find(fd) == fdMap_.end()){
        LOG_INFO("add new headContent:%d",fd);
        NodePtr ptr = std::make_shared<Node>();
        ptr->fd_ = fd;
        ptr->cPtr_ = cPtr;
        ptr->time_ = getNow();
        ptr->callback_ = std::move(callback);
        fdMap_.emplace(fd,ptr);
        NodeList_.emplace_front(ptr);
        NodeMap_.emplace(ptr,NodeList_.begin());
    } else{
        adjust(fd);
    }
}

void HeadDetection::adjust(int fd){
    assert(isSameThread(loop_->getThreadId()));
    LOG_INFO("adjust:%d",fd);
    if(fdMap_.find(fd) == fdMap_.end()){
        LOG_INFO("不存在fd");
    } else{
        LOG_INFO("存在fd");
        auto ptr = fdMap_[fd];
        ptr->time_ = getNow();
        auto it = NodeMap_[ptr];
        NodeList_.erase(it);
        NodeList_.emplace_front(ptr);
        NodeMap_[ptr] = NodeList_.begin();
    }
    // auto ptr = fdMap_[fd];
    // ptr->time_ = getNow();
    // auto it = NodeMap_[ptr];
    // NodeList_.erase(it);
    // NodeList_.emplace_front(ptr);
    // NodeMap_[ptr] = NodeList_.begin();
}

void HeadDetection::headRead(){
    assert(isSameThread(loop_->getThreadId()));
    struct timespec now;
    int ret = clock_gettime(CLOCK_REALTIME, &now);//获取时钟时间
    assert(ret != -1);
    while(!NodeList_.empty()){
        auto ptr = *NodeList_.rbegin();
        __time_t time = ptr->time_ + expire_;
        if(time < now.tv_sec){ // 未来的时间大才删除
            del(ptr);
        }
        else{
            break;
        }
    }
}

void HeadDetection::del(NodePtr ptr){
    assert(isSameThread(loop_->getThreadId()));
    LOG_INFO("timeout delete tcpClient");
    if(ptr->cPtr_.lock() != nullptr){
        LOG_INFO("timeout call closeBack");
        ptr->callback_();
    }
    destroyConnect(ptr);
}

void HeadDetection::destroyConnect(NodePtr ptr){
    assert(isSameThread(loop_->getThreadId()));
    LOG_INFO("destroyConnect");
    if(NodeMap_.find(ptr) != NodeMap_.end()){
        auto it = NodeMap_[ptr];
        NodeList_.erase(it);
        fdMap_.erase(ptr->fd_);
        NodeMap_.erase(ptr);
    }
}