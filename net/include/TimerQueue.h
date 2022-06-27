//
// Created by cy & dxgzg on 2022/6/25.
//

#ifndef PINE_TIMERQUEUE_H
#define PINE_TIMERQUEUE_H
#include <set>
#include <vector>
#include <string>
#include <memory>

#include "const.h"
#include "nocopyable.h"

class Timer;
class Channel;
class EventLoop;

// todo
class TimerQueue :public nocopyable{
    using timerQueue = std::set<std::pair<std::uint64_t,std::unique_ptr<Timer>>>;
public:
    TimerQueue(EventLoop* loop);
    ~TimerQueue();
    Timer* addTimer(timerCallback timerCallback,int expire,bool repeat = false);
    void cancelTimer(Timer*);
private:
    void handleRead();
    void insert(std::unique_ptr<Timer>&);
private:
    EventLoop* loop_;
    int timefd_;
    std::unique_ptr<Channel> timerChannel_;
    timerQueue timerQueue_;
};


#endif //PINE_TIMERQUEUE_H
