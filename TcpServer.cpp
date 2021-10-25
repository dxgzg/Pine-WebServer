#include "TcpServer.h"
#include "EventLoop.h"
#include "Logger.h"
#include "EventLoopThreadPool.h"
#include "HeartConnect.h"

#include <thread>
using namespace std;

TcpServer::TcpServer(EventLoop* loop):
            loop_(loop),
            inetAddr_(std::make_unique<InetAddr>()),
            acceptor_(std::make_unique<Acceptor>(inetAddr_.get())),
            threadPool_(std::make_unique<EventLoopThreadPool>())
 {
    initChannel();
}

// 允许用户自定义一个IP和port
TcpServer::TcpServer(EventLoop* loop,
                    InetAddr* inetAddr):loop_(loop),inetAddr_(inetAddr)
                    ,threadPool_(std::make_unique<EventLoopThreadPool>())
                    
{   
    inetAddr_ = std::unique_ptr<InetAddr>(inetAddr);
    acceptor_ = std::make_unique<Acceptor>(inetAddr_.get());
    initChannel();
}


void TcpServer::accept(){
    int connfd = acceptor_->accept();
    LOG_INFO("main accept new connect");
    EventLoop* loop = threadPool_->getNextLoop();
    Pine::clientPtr t(new TcpClient(loop,connfd));
    std::function<void(Pine::clientPtr&)>fun = std::bind(&TcpServer::handleClose,this,std::placeholders::_1);
    t->setTcpServerCloseCallback(fun);
    t->setReadCallback(readCallback_);
    clientMap_[connfd] = t;
    loop->runInLoop(std::bind(&TcpClient::registerClient,t.get()));
    
}

void TcpServer::handleNewClient(){
    this->accept();
}

// 主线程去删除这些客户数据
void TcpServer::closeInLoop(Pine::clientPtr& t){
    // LOG_INFO("closeInLoop delete thread id:%zu",std::this_thread::get_id());
    
    int closeFd = t->getFd();
    if(clientMap_.find(closeFd) != clientMap_.end()){
        clientMap_.erase(closeFd);
    }
    LOG_INFO("closeInLoop use count:%ld",t.use_count());
}   

// 子线程处理完给主线程处理。
void TcpServer::handleClose(Pine::clientPtr& t){
    // LOG_INFO("handleClose delete thread id:%zu",std::this_thread::get_id());

    LOG_INFO("handleClose use count:%ld",t.use_count());
    loop_->runInLoop(std::bind(&TcpServer::closeInLoop,this,t));
} 

void TcpServer::start(){
    threadPool_->start();
    LOG_INFO("服务器启动成功,%s",inetAddr_->getIpPort().c_str());
}

void TcpServer::initChannel(){
    channel_ = std::make_unique<Channel>(loop_,acceptor_->getFd());
    channel_->setReadCallback(std::bind(&TcpServer::handleNewClient,this));
    channel_->enableReadEvent();
}


TcpServer::~TcpServer(){}

void TcpServer::setThreadNum(int num){
    threadPool_->setThreadNum(num); 
}