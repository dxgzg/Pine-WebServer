#ifndef SOCKET_H_INCLUED
#define SOCKET_H_INCLUED

class Socket
{
public:
    Socket(int sockfd_);
    bool setKeepAlive(bool on);
    bool setReusePort(bool on);
    bool setReuseAddr(bool on);
    int getFd()const{return sockfd_;}
    int accept();
    ~Socket();
private:
    int sockfd_;
};

#endif