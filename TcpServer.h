#pragma once

#include <memory>
#include <functional>
#include <unordered_map>


#include "TcpClientCallback.h"
#include "Channel.h"
#include "Acceptor.h"
#include "InetAddr.h"
#include "TcpClient.h"


class EventLoop;
class TcpClient;
class EventLoopThreadPool;

class TcpServer{
private: 
    using clientMap = std::unordered_map<int,Pine::clientPtr>;
    
public:
    TcpServer(EventLoop* loop);
    TcpServer(EventLoop* loop,InetAddr* inetAddr);
    //TcpServer(EventLoop* loop,InetAddr* inetAddr,TcpClientReadCallBackFun readcb);
    int getFd()const{return acceptor_->getFd();}
    void accept();
    void start();
    void handleNewClient();
    void handleClose(Pine::clientPtr&);
    void closeInLoop(Pine::clientPtr&);
    void setThreadNum(int);
    void setClientReadCallback(Pine::TcpClientReadCallBackFun readCallback){readCallback_ = readCallback;}
    ~TcpServer();
private:
    // 注册绑定回调函数S
    void initChannel();
private: 
    EventLoop* loop_;
    std::unique_ptr<InetAddr> inetAddr_;
    std::unique_ptr<Acceptor> acceptor_;

   
    
    std::unique_ptr<EventLoopThreadPool> threadPool_;
    std::unique_ptr<Channel> channel_;
    clientMap clientMap_;
    
private:
    Pine::TcpClientReadCallBackFun readCallback_ = nullptr;
};