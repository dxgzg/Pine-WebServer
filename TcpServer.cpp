#include"TcpServer.h"
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <assert.h>
#include <iostream>
#include <functional>
#include <signal.h>
#include <unistd.h>
#include "Safe_queue.h"
#include "Timer.h"
using std::function;
using std::cout;
using std::endl;
constexpr int MAX_EVENT_NUM = 1024;

bool timeout_flag = false;
int epollfd2 = 0;
void sig_alrm(int){
    Timer::get_Timer()->tick(epollfd2);
    timeout_flag = false;
}

TcpServer::TcpServer(std::shared_ptr<Safe_queue> sf_sqe,fun_cb read,fun_cb write){
    this->sf_que = sf_sqe;
    cout<<"tcp server sf_que use num "<<this->sf_que.use_count()<<endl;
    callback = std::make_shared<cb>();
    callback->read = read;
    callback->write = write;
    //timer = std::make_shared<Timer>();
}
bool TcpServer::createSockBindListen(int family,const char* ip,int port,int listen_num){
    listen_fd = socket(family, SOCK_STREAM | SOCK_NONBLOCK ,0);
    cout<<"IPPROTO_TCP "<<IPPROTO_TCP<<endl;
    int ret = -1;
   //setsockopt(listen_fd,SOL_SOCKET,SO_REUSEPORT,&ret,sizeof(ret));//测试的时候开了还是挺方便的
    if(listen_fd < 0){
        cout<<"socket 创建失败"<<endl;
        return false;
    }
    sockaddr_in addr;
    addr.sin_family = family;
    addr.sin_port = htons(port);
    inet_pton(family,ip,&addr.sin_addr);
    int res = bind(listen_fd,(sockaddr*)&addr,sizeof(addr));
    if(res < 0){
        cout<<"bind 绑定失败"<<endl;
        return false;
    }
    res = listen(listen_fd,listen_num);
    if(res < 0){
        cout<<"listen 监听失败"<<endl;
        return false;
    }
    epollfd = epoll_create(16);
    if(epollfd < 0){
        cout<<"epoll 创建失败"<<endl;
        return false;
    }
    epollfd2 = epollfd;
    cout<<"服务器启动成功"<<endl;
    if(signal(SIGALRM,sig_alrm) < 0){
        cout<<"sig_alrm error"<<endl;
        return -1;
    }
    epoll_event events[MAX_EVENT_NUM];
    epoll_event listen_event;
    listen_event.data.fd = listen_fd;
    listen_event.events = EPOLLIN;
    epoll_ctl(epollfd,EPOLL_CTL_ADD,listen_fd,&listen_event);


    while(1){
        if(!timeout_flag)//只有这样才可以触发定时信号
        {
            alarm(TIME + 3);//给个3秒缓冲
            timeout_flag = true;
        }
        int ready = epoll_wait(epollfd,events,MAX_EVENT_NUM,-1);//-1永不超时
        if(ready < 0){
            if(errno == EINTR){//超时引起的慢启动中断
                // cout<<"eintr"<<endl;
                // Timer::get_Timer()->tick(epollfd);
                // timeout_flag = false;
                continue;
            }
            else{
                cout<<"epoll wait error"<<errno<<endl;
                break;
            }
        }
        for(int i = 0;i < ready;++i){
            int connfd = events[i].data.fd;
            if(connfd == listen_fd){
                cout<<"有连接接入"<<endl;
                int client_fd = accept(listen_fd,nullptr,nullptr);
                cout<<"fd : "<<client_fd<<endl;
                epoll_event client_event;
                client_event.data.fd = client_fd;
                client_event.events = EPOLLIN | EPOLLET;
                epoll_ctl(epollfd,EPOLL_CTL_ADD,client_fd,&client_event);

                Timer::get_Timer()->add_time_obj(client_fd);
            }
            //其他事情交给线程去做了
            else if(events[i].events & EPOLLIN){//处理读
                Timer::get_Timer()->adjust_obj(connfd);
                sf_que->addEvent(callback->read,connfd,nullptr);
            }
            else{//处理写
                //sf_que->addEvent(callback->write,events[i].data.fd,nullptr);
            }
        }

    }
    return true;
}