#ifndef PINE_TIMER_H
#define PINE_TIMER_H
#include "nocopyable.h"
#include "const.h"

#include <string>
#include <any>


class Timer{
public:
    explicit Timer(std::uint64_t   when,std::uint64_t expire,
          timerCallback timerCallback,bool repeat = false);
    void run(){timerCallback_();}
//    std::uint64_t getTimerId(){return timerId_;}
    bool isTimerRepeat(){return repeat_;}
    std::uint64_t expiration(){return expire_;}
    std::uint64_t when(){return when_;}
    bool repeat(){return repeat_;}
    void setWhen(int seconds){when_ += seconds;}
private:
    std::uint64_t   when_; // 到什么时候，已经加上了expire_ time了
    std::uint64_t   expire_;
    timerCallback   timerCallback_;
    bool            repeat_;
};


#endif //PINE_TIMER_H
