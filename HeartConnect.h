#pragma once
#include <list>
#include <unordered_map>
#include <functional>
#include <memory>

#include "nocopyable.h"
#include "Logger.h"

class Channel;
class EventLoop;
class TcpClient;

struct Node{
    __time_t time_ = 0;
    int fd_ = 0;
    std::weak_ptr<TcpClient> cPtr_;
    std::function<void()> callback_;
    bool operator<(const std::shared_ptr<Node>& lhs)const {return this->time_ < lhs->time_;}
    ~Node(){
        LOG_INFO("head detection delete");
    }
};
/*
*   心跳包
*   插入头，删除尾
*   
*/
class HeartConnect : public nocopyable{
    using NodePtr = std::shared_ptr<Node>;
    
public:
    HeartConnect(EventLoop* loop,int expire = 1000000);
    ~HeartConnect() = default;
    void add(int fd,std::shared_ptr<TcpClient> cPtr,std::function<void()> callback);
    // 处理定时产生的消息
    void headRead();
   
    int getTimeFd()const{return timefd_;}

    NodePtr getNodePtr(int fd){return fdMap_[fd];}

    void clientCloseCallback(int fd);
private:
    // 主动关闭连接，需要调用回调函数
    void del(NodePtr ptr);  // 只有这个类可以调用
    void destroyConnect(NodePtr ptr);
    void adjust(int fd);
private:
    std::list<NodePtr> NodeList_; // 保证插入的顺序
    std::unordered_map<NodePtr,std::list<NodePtr>::iterator> NodeMap_;
    // key fd value Node*
    std::unordered_map<int,NodePtr> fdMap_;
    int timefd_;
    // 定时时长，这里是固定的
    int expire_;
    EventLoop* loop_;
    std::unique_ptr<Channel> channel_;
};