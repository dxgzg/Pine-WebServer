#pragma once
#include <thread>


class EventLoop;

class Thread
{
private:
public:
    Thread(EventLoop* loop);
    void start();

    ~Thread() = default;
private:
    EventLoop* loop_;
    std::thread thread_;
};

