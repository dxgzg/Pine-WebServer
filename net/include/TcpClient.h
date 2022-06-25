#pragma once
#include <memory>
#include <string>
#if 1
    #include <chrono>
    
#endif

#include "Channel.h"
#include "TcpClientCallback.h"
#include "Socket.h"
#include "const.h"


class Buffer;
class EventLoop;
class HttpInfo;

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
    void setHeartConnectCloseCallback(const std::function<void(int)>& cb){
        heartConnectCloseCallback_ = cb;
    }
    void setReadCallback(Pine::TcpClientReadCallBackFun& cb){readCallback_ = cb;}
    int getFd()const{return clientFd_->getFd();}
    EventLoop* getLoop(){return loop_;}
    Channel* getChannel()const{return channel_.get();}

    int sendInLoop(std::string&);
    int getState()const{return (int)state_;}
    void setState(CLIENT_STATUS state){state_ = state;}
    std::unique_ptr<HttpInfo>& getHttpInfo(){return httpInfo_;}
//    std::unique_ptr<HttpInfo>& resetHttpInfo();

    void readOk(int len);
private:
    std::unique_ptr<Socket> clientFd_;
    std::unique_ptr<Channel> channel_; 
    EventLoop* loop_;
    Pine::TcpClientReadCallBackFun readCallback_;
    closeFun closeCallback_;
    std::unique_ptr<Buffer> inputBuffer_;
    std::unique_ptr<Buffer> outputBuffer_;
    CLIENT_STATUS state_;
    std::function<void(int)> heartConnectCloseCallback_;
    std::unique_ptr<HttpInfo> httpInfo_;

    // todo 待修改
    #if 1
    public:
        std::chrono::time_point<std::chrono::system_clock> start;
    #endif
};
//todo 完善客户端的状态机