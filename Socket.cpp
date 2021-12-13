#include "Logger.h"
#include "Socket.h"


#include <sys/types.h>      
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <iostream>
using namespace std;

Socket::Socket(int sockfd){
    sockfd_ = sockfd;
}

bool Socket::setKeepAlive(bool on){
    int opt = on ? 1 : 0;
    int ret = ::setsockopt(sockfd_,SOL_SOCKET,SO_KEEPALIVE,&opt,sizeof(opt));
    return ret == 0? true:false;
}

bool Socket::setReusePort(bool on){
    int opt = on ? 1 : 0;
    int ret = ::setsockopt(sockfd_,SOL_SOCKET,SO_REUSEPORT,&opt,sizeof(opt));
    return ret == 0? true:false;
}

bool Socket::setReuseAddr(bool on){
    int opt = on ? 1 : 0;
    int ret = ::setsockopt(sockfd_,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt));
    return ret == 0? true:false;
}

int Socket::accept(){
    // 把连接的套接字也设置成非阻塞的
    int connfd = ::accept4(sockfd_,nullptr,nullptr,SOCK_NONBLOCK | SOCK_CLOEXEC);
    // int connfd = ::accept4(sockfd_,nullptr,nullptr,SOCK_CLOEXEC);
    if(connfd < 0){
        LOG_ERROR("accept error: %d",errno);
        return -1;
    }

    return connfd;
 }

 Socket::~Socket(){
     ::close(sockfd_);
 }