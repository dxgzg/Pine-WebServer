#include "EventLoop.h"
#include "TimerQueue.h"
#include "Logger.h"
#include "TimeStamp.h"
#include <iostream>
#include <thread>

using namespace std;

void myTimer(){
    cout << "myTimer" << endl;
}

// 添加多个计时器。
void example_multi_add_timer(){
    EventLoop loop;
    TimerQueue q(&loop);

    q.addTimer(myTimer,7);
    LOG_INFO("time");
    this_thread::sleep_for(4000ms);
    q.addTimer(myTimer,2);
    LOG_INFO("hello timer test");
//    while(1){
//
//    }
    loop.loop();
}

// 测试重复定时，如心跳包
void example_repeat_timer(){
    EventLoop loop;
    TimerQueue q(&loop);
    LOG_INFO("start");
    q.addTimer(myTimer,2,true);
    loop.loop();
}

int main() {
    example_multi_add_timer();
//    example_repeat_timer();

    EventLoop loop;
    TimerQueue q(&loop);

    loop.loop();
    return 0;

}