//
// Created by cy & dxgzg on 2022/6/25.
//

#include "TimerQueue.h"
#include "Channel.h"
#include "EventLoop.h"
#include "Timer.h"
#include "Logger.h"
#include "TimeStamp.h"

#include <sys/timerfd.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>

using namespace std;

inline void memZero(void* p, size_t n)
{
    memset(p, 0, n);
}

__time_t getNow(){
    timespec now;
    int ret = clock_gettime(CLOCK_REALTIME, &now);//获取时钟时间
    assert(ret != -1);
    return now.tv_sec;
}

int createTimefd(){
    int timefd =  timerfd_create(CLOCK_REALTIME, TFD_NONBLOCK);
    if(timefd < -1){
        LOG_FATAL("time fd error");
    }
    return timefd;
}

void resetTime(int timefd,std::uint64_t when){
    __time_t expire = when - TimeStamp::nowToSecond();
    struct itimerspec newValue;
    struct itimerspec oldValue;
    memZero(&newValue, sizeof newValue);
    memZero(&oldValue, sizeof oldValue);
    newValue.it_value.tv_sec = expire;

    int ret = timerfd_settime(timefd, 0, &newValue, NULL);//启动定时器
    assert(ret != -1);
}

void readTimerfd(int timerfd) {
    uint64_t howmany;
    ssize_t n = ::read(timerfd, &howmany, sizeof howmany);
    assert(n != -1);
}

TimerQueue::TimerQueue(EventLoop* loop):loop_(loop),timefd_(createTimefd()),
                        timerChannel_(std::make_unique<Channel>(loop,timefd_)),
                        timerQueue_(){
    timerChannel_->setReadCallback(std::bind(&TimerQueue::handleRead,this));
    timerChannel_->enableReadEvent();
}

TimerQueue::~TimerQueue() = default;

Timer* TimerQueue::addTimer(timerCallback timerCallback,int expire,bool repeat){
    LOG_INFO("insert");
    LOG_INFO("%ld,%d，%ld",TimeStamp::nowToSecond(),expire,expire + TimeStamp::nowToSecond());
    unique_ptr<Timer> timer = make_unique<Timer>(expire + TimeStamp::nowToSecond(),expire,timerCallback,repeat);
    auto result = timer.get();
    insert(timer);

    return result;
}

void TimerQueue::insert(std::unique_ptr<Timer>& timer){
    auto it = timerQueue_.begin();
    // empty or need reset setTime
    if(it == timerQueue_.end() || it->first > timer->when()){
        resetTime(timefd_,timer->when());
    }
    timerQueue_.emplace(make_pair(timer->when(),std::move(timer)));
}


void TimerQueue::cancelTimer(Timer*){

}

void TimerQueue::handleRead(){
    readTimerfd(timefd_);
    // todo handleExpire
    LOG_INFO("time out");

    std::uint64_t now = TimeStamp::nowToSecond();
    for(auto it = timerQueue_.begin();it != timerQueue_.end();){
        LOG_INFO("222");
        if(it->first <= now){
            it->second->run(); // callback
            if(it->second->repeat()){
                it->second->setWhen(it->second->expiration());
                auto timer = make_unique<Timer>(*it->second);
                insert(timer);
            }
            it = timerQueue_.erase(it);
        } else{
            break;
        }
        LOG_INFO("1111");
    }

    // todo reset time

    auto it = timerQueue_.begin();
    if(it != timerQueue_.end()){
        resetTime(timefd_,it->first);
    }
}

