
#include <sys/epoll.h>
#include <iostream>
#include <chrono>
#include <unistd.h>
#include <time.h>
#include <sys/socket.h>
#include "Timer.h"
using namespace std;
Timer* Timer::t_out = new Timer();
void Timer::del_Timer(){
    delete t_out;
}
void Timer::add_time_obj(int connfd){
        
    sp_init_tm ptr = make_shared<initial_timer>();
	ptr->connfd = connfd;
    t_out->settime(ptr);

    //把新的连接添加到链表的头部和哈希表中
    l.emplace_front(ptr);
    um.emplace(connfd,l.begin());
        
}
void Timer::adjust_obj(int connfd){
    //肯定是有这个套接字才能触发这个函数
    auto it = um.find(connfd);
    sp_init_tm tmp = *it->second;
    l.erase(it->second);
    //重新设置一下时间
    this->settime(tmp);
    //统一插到头部上面去
    l.emplace_front(tmp);
    um[connfd] = l.begin();
    cout<<"adjust_obj"<<endl;
}
 //超时、正常关闭都需要调用这个函数
void Timer::del_time_obj(int event_base,int connfd){
    auto it = um.find(connfd);
    int ctl = epoll_ctl(event_base,EPOLL_CTL_DEL,it->first,nullptr);
    if(ctl < 0){
        cout<<"del_time_obj ctl error"<<endl;
        return ;
    }
    //清理信息
    l.erase(it->second);
    um.erase(connfd);
    int num = close(connfd);
    cout<<"close "<< num <<endl;
}

void Timer::tick(int event_base){
   // cout<<"in tick "<<l.size()<<endl;
    time_t cur = time(nullptr);
    auto it = l.rbegin();
    while(it != l.rend()){
        sp_init_tm tmp = *it;
        if(cur > tmp->expire){//超时了，删掉他
            //让del函数去重新给it赋值
            this->del_time_obj(event_base,tmp->connfd);
            it = l.rbegin();
        }
        else{
            break;
        }
    }
}

Timer* Timer::get_Timer(){    
    return t_out;
}