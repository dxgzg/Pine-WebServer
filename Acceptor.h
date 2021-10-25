#pragma once

class Socket;
class InetAddr;

class Acceptor{
public:
    Acceptor(InetAddr* inetAddr);
    ~Acceptor();
    int accept();
    int getFd()const{return serverSock_;}
private:
    int serverSock_;
    Socket* socket_;
    InetAddr* inetAddr_;
    
};