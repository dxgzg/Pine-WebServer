#pragma once
#include <string>
#include <arpa/inet.h>
class InetAddr{
public:
    InetAddr(int family = AF_INET);
    bool sockBindListen(int serverFd);
    void setIp(std::string ip){ip_ = ip; }
    void setPort(int port){port_ = port;}
    std::string getIpPort();
private:
    sockaddr_in addr_;
    std::string ip_;
    int port_;


};