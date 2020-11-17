#include "Pine_Base.h"
#include "Safe_queue.h"
#include "Thread_pool.h"
#include "TcpServer.h"
#include <arpa/inet.h>
#include <string.h>
#include <signal.h>
using namespace std;
//单例模式
//unique_ptr<Pine_Base> Pine_Base::pine_base = make_unique<Pine_Base>(nullptr);

Pine_Base::Pine_Base(fun_cb read,fun_cb write){
    sf_que = make_shared<Safe_queue>();
    _thread = make_shared<Thread_pool>(sf_que);
    this->tcp_server = make_shared<TcpServer>(sf_que,read,write);
    if(signal(SIGPIPE,SIG_IGN) <0){
        cout<<"管道忽略错误"<<endl;
    }
}

bool Pine_Base::eventLoop(sockaddr_in* addr){
    char str[16] = {0};
    inet_ntop(addr->sin_family,&addr->sin_addr,str,sizeof(str));
    if(str == nullptr){//相等返回零
        return false;
    }
    bool flag = tcp_server->createSockBindListen(addr->sin_family,str,addr->sin_port);
    if(!flag){//出错了的话
        return false;
    }
    return true;
}

bool Pine_Base::eventLoop(int family,const char*ip,int port){
    cout<<"sf_que use num"<< sf_que.use_count()<<endl;
    bool flag = tcp_server->createSockBindListen(family,ip,port);
    if(!flag){//出错了的话
        return false;
    }
    return true;
}