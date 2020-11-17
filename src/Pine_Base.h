#ifndef PINE_BASE__H_INCLUDED
#define PINE_BASE__H_INCLUDED
#include <sys/epoll.h>
#include <functional>
#include <memory>
#include <netinet/in.h>

constexpr int ET = 1;
class Safe_queue;
class Thread_pool;
class TcpServer;
class Pine_Base
{ 
    using fun_cb = std::function<void(int,void*)>;
private:
    //static std::unique_ptr<Pine_Base> pine_base;//单例模式
    std::shared_ptr<TcpServer> tcp_server;
    std::shared_ptr<Safe_queue> sf_que;
    std::shared_ptr<Thread_pool> _thread;
public:
    Pine_Base(fun_cb read,fun_cb write);
    //bool是为了自动调用sock还是客户自己设置
    bool eventLoop(sockaddr_in*);
    bool eventLoop(int family,const char*ip,int port);
    ~Pine_Base() = default;
};
#endif