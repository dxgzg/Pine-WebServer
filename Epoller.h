#pragma once

#include <sys/epoll.h>
#include <vector>
#include <memory>

#include "Poller.h"
class Channel;
enum class State{
    ADD = -1,
    DEL = 1, // 只有添加过了，再触发才是del
};
class Epoller : public Poller{
    enum class State{
        NEW = 0,
        MOD = 1,
        DEL = 2, // 只有添加过了，再触发才是del
    };
public:
    explicit Epoller();
    void poll(std::vector<Channel*>& artiveEvent)override;
    void updateEvent(int opt,Channel* channel);
    void updateChannel(Channel* channel);
    ~Epoller() = default;
private:
    
    std::vector<epoll_event> artiveEvent_;
    int epollfd_;
    epoll_event events_;
    
};