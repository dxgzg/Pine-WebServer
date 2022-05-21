#include "TcpClient.h"
#include "EventLoop.h"
#include "Logger.h"
#include "Buffer.h"
#include "HeartConnect.h"
#include "HttpParse.h"

#if 1
    #include <chrono>
    using namespace std::chrono;
#endif

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
            state_(CLIENT_STATUS::NONE),
            httpInfo_(make_unique<HttpInfo>())
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
    this->state_ = CLIENT_STATUS::CONNECT;
    channel_->enableReadEvent();
}

void TcpClient::CloseCallback(){
    assert(state_ !=  CLIENT_STATUS::DISCONNECT );
    // 后者是防止在发送消息的时候超时事件的发生
    if(state_ == CLIENT_STATUS::SEND_CONTINUE || state_ == CLIENT_STATUS::WAIT_DISCONNECT){
        state_ = CLIENT_STATUS::WAIT_DISCONNECT;
        return ;
    }

    LOG_INFO("fd:%d close callback",clientFd_->getFd());
    if(heartConnectCloseCallback_){
        heartConnectCloseCallback_(clientFd_->getFd());
    }

    channel_->setIndex(DELETE);// 准备删除操作。
    channel_->disableAll();
    state_ = CLIENT_STATUS::DISCONNECT;
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
            state_ = CLIENT_STATUS::SEND_ERROR;
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
    if(state_ == CLIENT_STATUS::DISCONNECT){
        LOG_INFO("客户已经断开连接了");
        return 0;
    }

    size_t n = outputBuffer_->send(clientFd_->getFd(),msg);
    LOG_INFO("send msg n:%zu",n);
    if(n == UINT64_MAX){
        LOG_ERROR("send error");
        // responst 调用关闭函数
//        CloseCallback();
        return -1;
    }

    auto headDet = loop_->getHeartConnect();
    headDet->add(clientFd_->getFd(),shared_from_this(),std::bind(&TcpClient::CloseCallback,this));

    if(msg.size() > n){
        string s = msg.substr(n);
        outputBuffer_->addMessage(const_cast<char*>(s.c_str()),s.size());
        state_ = CLIENT_STATUS::SEND_CONTINUE; // 设置有消息发送状态
        channel_->enableWriteEvent();

    } else{
        channel_->disableWriteEvent();
    }

    
    return n;
}

int TcpClient::send(std::string msg){
    return sendInLoop(msg);
}

void TcpClient::sendExtra(){
    if(state_ == CLIENT_STATUS::DISCONNECT)return ;
    // LOG_INFO("send extra");
    // channel可写事件的回调函数。
    string msg = outputBuffer_->getAllString();
    
    size_t n = outputBuffer_->send(clientFd_->getFd(),msg);
    
    if(n == UINT64_MAX){
        state_ = CLIENT_STATUS::SEND_ERROR;
        CloseCallback();
        return ;
    }
    outputBuffer_->retrieve(n);

    if(n == msg.size() && outputBuffer_->readAble() == 0){
        auto headDet = loop_->getHeartConnect();
        headDet->add(clientFd_->getFd(),shared_from_this(),std::bind(&TcpClient::CloseCallback,this));
        channel_->disableWriteEvent();

//        auto end = system_clock::now();
//        auto duration = duration_cast<microseconds>(end - start);
//        cout <<  "花费了"
//        << double(duration.count()) * microseconds::period::num / microseconds::period::den
//        << "秒" << endl;

        // 如果是等待关闭状态，关掉这个连接
        if(state_ == CLIENT_STATUS::WAIT_DISCONNECT){
            state_ = CLIENT_STATUS::SEND_OK;
            CloseCallback();
        }
    }
    else{
        string s = msg.substr(n);
        outputBuffer_->addMessage(const_cast<char*>(s.c_str()),s.size());
    }
}

//std::unique_ptr<HttpInfo>& TcpClient::resetHttpInfo(){
//    // httpInfo_ = make_unique<HttpInfo>();
//     if(status_ == PARSE_STATUS::PARSE_OK){
//        httpInfo_->reset();
//     }
//    return httpInfo_;
//}
//
//void TcpClient::setParseStatus(PARSE_STATUS s){
//    status_ = s;
//}

void TcpClient::readOk(int len){
    inputBuffer_->retrieve(len);
}