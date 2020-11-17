#ifndef SOCKET_H_INCLUED
#define SOCKET_H_INCLUED
#include <netinet/in.h>
class Socket
{
private:
    int listen_fd = -1;
public:
    Socket(/* args */);
    bool sock(int family);
    bool bind_(const char* ip,int port);
    bool bind_(sockaddr_in*);
    bool listen_(int listen_num =5);

    ~Socket();
};

#endif