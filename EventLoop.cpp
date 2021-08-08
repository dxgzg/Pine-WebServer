#include <sys/eventfd.h>
#include <errno.h>
#include <unistd.h>
#include <assert.h>


#include "EventLoop.h"
#include "Epoller.h"
#include "Channel.h"
#include "Logger.h"
#include "HeadDetection.h"

#include <iostream>
using namespace std;


static int createFd(){
    int evtfd = ::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
	if (evtfd < 0) {
		LOG_FATAL("eventfd error:%d \n", errno);
	}
	return evtfd;
}

static bool isSameThread(const std::thread::id& thread_id){
    return thread_id == std::this_thread::get_id();
}

EventLoop::EventLoop():epoller_(std::make_unique<Epoller>()),
                        wakefd_(createFd()),
                        threadId_(std::this_thread::get_id()),
                        wakeChanel_(std::make_unique<Channel>(this,wakefd_)),
                        headDet_(std::make_unique<HeadDetection>(this))
{
    // unsigned int t = *(unsigned int*)&threadId_;
    //LOG_INFO("loop成员变量线程id: %u",t);

    wakeChanel_->setReadCallback(std::bind(&EventLoop::handleRead,this));
    wakeChanel_->enableReadEvent();
}

void EventLoop::loop(){
    while(true){
        std::vector<Channel*> artiveChannel;
        epoller_->poll(artiveChannel);
        for(size_t i = 0;i < artiveChannel.size();++i){
            artiveChannel[i]->handleEvent();// channel去处理事件了
        }
        doPendingFunctor();
    }
}

void EventLoop::updateChannel(Channel* channel){
    epoller_->updateChannel(channel);
}

void EventLoop::runInLoop(const Function& cb){
    if(::isSameThread(this->threadId_)){
        cb();
    }
    else{
        queueInLoop(cb);
    }
}


void EventLoop::queueInLoop(const Function& cb){
    {
        std::unique_lock<std::mutex>lock(this->mutex_);
        pendingFunctors_.emplace_back(cb);
    }
    if(!isSameThread(this->threadId_)){
        wakeup();
    }
}

void EventLoop::wakeup(){
    uint64_t one = 1;
    size_t n = ::write(wakefd_,&one,sizeof(one));
    if(n != sizeof(one)){
        LOG_ERROR("wakeup error");
    }
}
void EventLoop::handleRead(){
    uint64_t one = 1;
    size_t n = ::read(wakefd_,&one,sizeof(one));
    if(n != sizeof(one)){
        LOG_ERROR("handleRead not 8");
    }
}

void EventLoop::doPendingFunctor(){
    std::vector<Function> tmp;
    {
        std::unique_lock<std::mutex> lock(mutex_);
        tmp.swap(this->pendingFunctors_);
    }
    for(const auto& cb : tmp){
        cb();
    }
}


// void EventLoop::addHeadDetection(int fd,std::function<void()> callback){
//     if(::isSameThread(threadId_)){
//         headDet_->add(fd,callback);
//     }
//     else{
//         Function f = std::bind(&HeadDetection::add,headDet_.get(),fd,callback);
//         queueInLoop(std::move(f));
//     }
// }


EventLoop::~EventLoop(){

}