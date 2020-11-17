#ifndef TCPSERVER_H_INCLUDED
#define TCPSERVER_H_INCLUDED
#include <sys/epoll.h>
#include <memory>
#include <functional>
class Safe_queue;
class Timer;


using fun_cb = std::function<void(int,void*)>;
struct cb{//测试sizeof用的
    fun_cb read;
    fun_cb write;
};
/*
*暂时还没啥用
struct client_message{
    int fd = 0;
    int which = 0;//默认LT模式（水平触发），不是0就是ET模式
};
*/


class TcpServer
{
private:
    using EpollBase = int;
private:
    EpollBase epollfd = 0;
    int listen_fd = 0;
    std::shared_ptr<Safe_queue> sf_que;
    //std::shared_ptr<Timer> timer;
    std::shared_ptr<cb> callback;
public:
    TcpServer(std::shared_ptr<Safe_queue>,fun_cb read,fun_cb write);
    //true证明创建成功
    bool createSockBindListen(int family,const char* ip,int port,int listen_num = 5);

    ~TcpServer() = default;
};


#endif