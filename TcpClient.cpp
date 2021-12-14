#include "TcpClient.h"
#include "EventLoop.h"
#include "Logger.h"
#include "Buffer.h"
#include "HeartConnect.h"
#include "HttpParse.h"

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <assert.h>
using namespace std;
static constexpr int DELETE = 2;

static bool isSameThread(const std::thread::id& thread_id){
    return thread_id == std::this_thread::get_id();
}

TcpClient::TcpClient(EventLoop* loop,int clientFd)
            :clientFd_(new Socket(clientFd)),channel_(std::make_unique<Channel>(loop,clientFd)),
            loop_(loop),
            inputBuffer_(make_unique<Buffer>()),
            outputBuffer_(make_unique<Buffer>()),
            state_(STATE::CONNECT),
            httpInfo_(make_unique<HttpInfo>()),
            status_(PARSE_STATUS::PARSE_NONE)
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
    assert(isSameThread(loop_->getThreadId()));
    HeartConnect* heartConnect = loop_->getHeartConnect();

    std::function<void(int)> heartCallback = std::bind(&HeartConnect::clientCloseCallback,heartConnect,std::placeholders::_1);
    setHeartConnectCloseCallback(heartCallback);

    std::function<void()> callback = std::bind(&TcpClient::CloseCallback,this);
    heartConnect->add(getFd(),shared_from_this(),callback);

    channel_->enableReadEvent();
}

void TcpClient::CloseCallback(){
    LOG_INFO("fd:%d close callback",clientFd_->getFd());
    if(heartConnectCloseCallback_){
        heartConnectCloseCallback_(clientFd_->getFd());
    }
    channel_->setIndex(DELETE);// 准备删除操作。
    channel_->disableAll();
    auto ptr = shared_from_this();
    closeCallback_(ptr); // 这个是必定存在的
}
// 线程安全的
void TcpClient::ReadCallback(){
    assert(isSameThread(loop_->getThreadId()));
    LOG_INFO("fd:%d recv msg",clientFd_->getFd());
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
        auto headDet = loop_->getHeartConnect();
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
    if(getState() == (int)STATE::DISCONNECT){
        LOG_INFO("客户已经断开连接了");
        return 0;
    }
    size_t n = outputBuffer_->send(clientFd_->getFd(),msg);
    LOG_INFO("send msg n:%d",n);
    if(n == UINT64_MAX){
        state_ = STATE::DISCONNECT;
        this->setParseStatus(PARSE_STATUS::PARSE_OK);
        LOG_INFO("send error");
        CloseCallback();
        return -1;
    }
    auto headDet = loop_->getHeartConnect();
    headDet->add(clientFd_->getFd(),shared_from_this(),std::bind(&TcpClient::CloseCallback,this));
    if(msg.size() > n){
        this->setParseStatus(PARSE_STATUS::PARSE_CONTINUE);// 还需要继续发送
        string s = msg.substr(n);
        outputBuffer_->addMessage(const_cast<char*>(s.c_str()),s.size());
        channel_->enableWriteEvent();
    }
    this->setParseStatus(PARSE_STATUS::PARSE_OK);
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
    LOG_INFO("send extra");
    // channel可写事件的回调函数。
    string msg = outputBuffer_->getAllString();
    outputBuffer_->retrieve(msg.size());
    size_t n = outputBuffer_->send(clientFd_->getFd(),msg);
    auto headDet = loop_->getHeartConnect();
    headDet->add(clientFd_->getFd(),shared_from_this(),std::bind(&TcpClient::CloseCallback,this));
    if(n == UINT64_MAX){
        state_ = STATE::DISCONNECT;
        this->setParseStatus(PARSE_STATUS::PARSE_OK);
        CloseCallback();
        return ;
    }
    if(n == msg.size() && outputBuffer_->readAble() == 0){
        this->setParseStatus(PARSE_STATUS::PARSE_OK);
        channel_->disableWriteEvent();
    }
    else{
        this->setParseStatus(PARSE_STATUS::PARSE_CONTINUE);
        string s = msg.substr(n);
        outputBuffer_->addMessage(const_cast<char*>(s.c_str()),s.size());
    }
}

std::unique_ptr<HttpInfo>& TcpClient::resetHttpInfo(){
    // httpInfo_ = make_unique<HttpInfo>();
    // if(status_ == PARSE_STATUS::PARSE_OK){
        httpInfo_->reset();
    // }
    return httpInfo_;
}

void TcpClient::setParseStatus(PARSE_STATUS s){
    status_ = s;
}

void TcpClient::readOk(int len){
    inputBuffer_->retrieve(len);
}