#include "Socket.h"
#include <sys/types.h>      
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

bool Socket::sock(int family){
    this->listen_fd = socket(family,SOCK_STREAM,IPPROTO_TCP);
}