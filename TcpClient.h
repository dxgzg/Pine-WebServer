#pragma once
#include <memory>
#include <string>

#include "Channel.h"
#include "TcpClientCallback.h"
#include "Socket.h"

class Buffer;
class EventLoop;
enum class STATE{
    CONNECT = 0,
    DISCONNECT = 1
};
class TcpClient : public std::enable_shared_from_this<TcpClient>{
private:
    using closeFun = std::function<void(Pine::clientPtr&)>;
public:
    TcpClient(EventLoop* loop,int clientfd);
    ~TcpClient();

    void ReadCallback();
    void CloseCallback();
    void setCloseCallback();

    void registerClient();

    int send(std::string);
    void sendExtra();

    void setTcpServerCloseCallback(const closeFun& cb){ closeCallback_ = cb;}
    void setReadCallback(Pine::TcpClientReadCallBackFun& cb){readCallback_ = cb;}
    int getFd()const{return clientFd_->getFd();}
    EventLoop* getLoop(){return loop_;}
    Channel* getChannel()const{return channel_.get();}

    int sendInLoop(std::string&);
    int getState()const{return (int)state_;}
private:
    std::unique_ptr<Socket> clientFd_;
    std::unique_ptr<Channel> channel_;
    EventLoop* loop_;
    Pine::TcpClientReadCallBackFun readCallback_;
    closeFun closeCallback_;
    std::unique_ptr<Buffer> inputBuffer_;
    std::unique_ptr<Buffer> outputBuffer_;
    STATE state_;
};