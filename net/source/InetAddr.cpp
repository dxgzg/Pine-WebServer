#include "InetAddr.h"
#include "Logger.h"

#include <sys/types.h>          
#include <sys/socket.h>
#include <arpa/inet.h>
#include <assert.h>
#include <gflags/gflags.h>

static int listenNum = 1024;


DEFINE_string(ip,"127.0.0.1", "What ip to listen on");
DEFINE_int32(port, 9996, "What port to listen on");
InetAddr::InetAddr(int family)
    :addr_(),ip_(FLAGS_ip),port_(FLAGS_port)
{

}
bool InetAddr::sockBindListen(int serverFd){
    // bzero( &addr_, sizeof( addr_ ) );
    addr_.sin_family = AF_INET;
    addr_.sin_port = htons(port_);
    int res = ::inet_pton(AF_INET,ip_.c_str(),&addr_.sin_addr);
    // 1表示成功
    assert(res == 1);
    res = ::bind(serverFd,(sockaddr*)&addr_,sizeof(addr_));
    assert(res == 0);
    res = ::listen(serverFd,listenNum);
    if(res == -1){
        return false;
    }
    return true;
}

std::string InetAddr::getIpPort(){
    char ip[16] = { 0 };
    ::inet_ntop(AF_INET,&addr_.sin_addr,ip,20);
    int port = ntohs(addr_.sin_port);
    char ipPort[40] = { 0 };
    snprintf(ipPort,40,"ip: %s:%d",ip,port);
    return ipPort;
}
