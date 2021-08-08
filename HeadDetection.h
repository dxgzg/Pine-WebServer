#pragma once
#include <list>
#include <unordered_map>
#include <functional>
#include <memory>

class Channel;
class EventLoop;

struct Node{
    __time_t time_ = 0;
    int fd_ = 0;
    std::function<void()> callback_;
    bool operator<(const std::shared_ptr<Node>& lhs)const {return this->time_ < lhs->time_;}
    ~Node() = default;
};
/*
*   心跳包
*   插入头，删除尾
*   
*/
class HeadDetection{
    using NodePtr = std::shared_ptr<Node>;
public:
    HeadDetection(EventLoop* loop,int expire = 200);
    ~HeadDetection() = default;
    void add(int fd,std::function<void()> callback);
    // 处理定时产生的消息
    void headRead();

    void adjust(int fd);

    int getTimeFd()const{return timefd_;}

    // 到点删除函数
    void del(NodePtr ptr);
private:
    std::list<NodePtr> NodeList_;
    std::unordered_map<NodePtr,std::list<NodePtr>::iterator> NodeMap_;
    // key fd value Node*
    std::unordered_map<int,NodePtr> fdMap_;
    int timefd_;
    // 定时时长，这里是固定的
    int expire_;
    EventLoop* loop_;
    std::unique_ptr<Channel> channel_;
};