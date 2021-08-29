#include "TcpClient.h"
#include "EventLoop.h"
#include "Logger.h"
#include "Buffer.h"
#include "HeadDetection.h"


#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <assert.h>

static constexpr int DELETE = 2;

#if 1
#include <iostream>
#endif

using namespace std;

static bool isSameThread(const std::thread::id& thread_id){
    return thread_id == std::this_thread::get_id();
}

TcpClient::TcpClient(EventLoop* loop,int clientFd)
            :clientFd_(new Socket(clientFd)),channel_(std::make_unique<Channel>(loop,clientFd)),
            loop_(loop),
            inputBuffer_(make_unique<Buffer>()),
            outputBuffer_(make_unique<Buffer>()),
            state_(STATE::CONNECT)
{
    channel_->setReadCallback(std::bind(&TcpClient::ReadCallback,this));// 这是默认的，客户端也可以再次调用
    channel_->setCloseCallback(std::bind(&TcpClient::CloseCallback,this));
    channel_->setWriteCallback(std::bind(&TcpClient::sendExtra,this));
    readCallback_ = nullptr;
}
TcpClient::~TcpClient(){
    LOG_INFO("客户端退出连接");
}

// 线程安全注册回调函数
void TcpClient::registerClient(){
    
    channel_->enableReadEvent();
}

void TcpClient::CloseCallback(){
    LOG_INFO("%d close callback",clientFd_->getFd());
    auto headDet = loop_->getHeadDetection();
    auto nodePtr = headDet->getNodePtr(clientFd_->getFd());
    headDet->destroyConnect(nodePtr);
    channel_->setIndex(DELETE);// 准备删除操作。
    channel_->disableAll();
    auto ptr = shared_from_this();
    closeCallback_(ptr);
}
// 线程安全的
void TcpClient::ReadCallback(){
    assert(isSameThread(loop_->getThreadId()));
    LOG_INFO("%d recv msg",clientFd_->getFd());
    if(readCallback_){// 如果有就调用用户自定义的回调，没有的话，用这个; 
        int n = inputBuffer_->recvMsg(clientFd_->getFd());
        if(n <= 0){
            if(n == -1){
                LOG_ERROR("recv error");
            }
            state_ = STATE::DISCONNECT;
            CloseCallback();
            return ;
        }
        auto headDet = loop_->getHeadDetection();
        headDet->add(clientFd_->getFd(),shared_from_this(),std::bind(&TcpClient::CloseCallback,this));
        readCallback_(shared_from_this(),inputBuffer_.get());
        return ;
    }
    else{ // 默认的
    //     int fd = clientFd_->getFd();
    //     char buff[1024];
    //     //int r =1;
    //     int r = ::recv(fd,buff,sizeof(buff),0);
    //     if(r == 0){ // 处理关闭
    //         // 重新设置下状态，然后关闭
    //         CloseCallback();
    //         closeCallback_(shared_from_this());
    //         return ;
    //     }
    //     buff[r] = '\0';
    // //  LOG_INFO("收到一条消息:%s",buff);
    //     send(fd,buff,strlen(buff),0);
    }
}

// 线程安全的
int TcpClient::sendInLoop(std::string& msg){
    assert(isSameThread(loop_->getThreadId()));
    if(getState() == (int)STATE::DISCONNECT)return 0;
    size_t n = outputBuffer_->send(clientFd_->getFd(),msg);
    if(n == UINT64_MAX){
        state_ = STATE::DISCONNECT;
        CloseCallback();
    }
    auto headDet = loop_->getHeadDetection();
    headDet->add(clientFd_->getFd(),shared_from_this(),std::bind(&TcpClient::CloseCallback,this));
    if(msg.size() > n){
        // 这里有bug，应该进行一个写入缓冲区操作
        cout << __FUNCTION__ <<"的n"<< n << endl;
        string s = msg.substr(n);
        outputBuffer_->addMessage(const_cast<char*>(s.c_str()),s.size());
        channel_->enableWriteEvent();
    }
    return n;
}

int TcpClient::send(std::string msg){
    if(std::this_thread::get_id() == loop_->getThreadId()){
        return sendInLoop(msg);
    }
    else{
        loop_->queueInLoop(std::bind(&TcpClient::sendInLoop,this,std::ref(msg)));
    }
    return 0;
}

void TcpClient::sendExtra(){
    if(getState() == (int)(STATE::DISCONNECT))return ;
    // channel可写事件的回调函数。
    string msg = outputBuffer_->getAllString();
    size_t n = outputBuffer_->send(clientFd_->getFd(),msg);
    auto headDet = loop_->getHeadDetection();
    headDet->add(clientFd_->getFd(),shared_from_this(),std::bind(&TcpClient::CloseCallback,this));
    cout << "dump before n length:" << n << endl; 
    if(n == UINT64_MAX){
        state_ = STATE::DISCONNECT;
        CloseCallback();
        return ;
    }
    if(n == msg.size() && outputBuffer_->readAble() == 0){
        channel_->disableWriteEvent();
    }
    else{
        cout << __FUNCTION__ << "的n:"<<n<<"msg size:"<<msg.size()<< endl;
        cout << "没想到吧，我还在执行" << endl;
        string s = msg.substr(n);
        outputBuffer_->addMessage(const_cast<char*>(s.c_str()),s.size());
    }
    if(n == UINT64_MAX){
        CloseCallback();
    }
    cout << "=====================" << endl;
    cout << "我还可以再战！！！！！！！！ " << endl;
    cout << "=====================" << endl;
}
