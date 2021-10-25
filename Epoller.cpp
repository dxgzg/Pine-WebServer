#include "Epoller.h"
#include "Channel.h"
#include "Logger.h"

#include <functional>
#include <signal.h>

Epoller::Epoller():artiveEvent_(16),epollfd_(::epoll_create(8)){}

void Epoller::poll(std::vector<Channel*>& artiveEvent){
    size_t num = ::epoll_wait(epollfd_,&(*artiveEvent_.begin()),artiveEvent_.size(),-1);
    if(num == UINT64_MAX){
        LOG_ERROR("epoll_wait errno %d",errno);
    }
    else if(num == 0){
        LOG_INFO("Time Out %s",__func__);
    }
    // 自动扩容
    if(num >= artiveEvent_.size()){
        artiveEvent_.resize(artiveEvent_.size() * 2);
    }
    // 找出活跃的事件
    for(size_t i=0;i < num ;++i){
        Channel* ptr = (Channel*)artiveEvent_[i].data.ptr;
        if(ptr == nullptr){
            LOG_INFO("该客户已断开");
            return ;
        }
        // 把触发的事件也添加进去
        ptr->setCurEventState(artiveEvent_[i].events);
        artiveEvent.emplace_back(ptr);
    }
}

void Epoller::updateEvent(int opt,Channel* channel){
    epoll_event event;
    event.data.fd = channel->getFd();
    event.data.ptr = channel;
    event.events = channel->getEvents();
    //event.events = EPOLLIN | EPOLLHUP; // 这里要改一下的，先处理一个close
    ::epoll_ctl(epollfd_,opt,channel->getFd(),&event);
}

void Epoller::updateChannel(Channel* channel){
    int index = channel->getIndex();
    if( index == (int)State::NEW){
        // 下一次就是该修改或者删除了
        channel->setIndex((int)State::MOD); 
        updateEvent(EPOLL_CTL_ADD,channel);
        channels_.emplace(channel->getFd(),channel);
    }
    else if(index == (int)State::DEL){
        updateEvent(EPOLL_CTL_DEL,channel);
        channels_.erase(channel->getFd());
    }
    else{
        updateEvent(EPOLL_CTL_MOD,channel);
    }
}