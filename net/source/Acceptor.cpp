#include "Acceptor.h"
#include "Socket.h"
#include "Logger.h"
#include "InetAddr.h"

#include <unistd.h>

static int createSocket(){
    int fd = ::socket(AF_INET,SOCK_NONBLOCK | SOCK_CLOEXEC | SOCK_STREAM,0);
    return fd;
} 

Acceptor::Acceptor(InetAddr* inetAddr):serverSock_(createSocket()),
    socket_(new Socket(serverSock_)),inetAddr_(inetAddr)
{
    socket_->setReusePort(true);
    inetAddr_->sockBindListen(serverSock_);
    socket_->setReuseAddr(true);
}

Acceptor::~Acceptor(){
    delete socket_;
    ::close(serverSock_);
}

int Acceptor::accept(){
    int connfd = socket_->accept(); // 监听事件
    return connfd;
}