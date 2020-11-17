#include"Safe_queue.h"
#include<iostream>


using std::mutex;
using std::unique_lock;

void Safe_queue::addFd(int fd,void*args){
    client_fd.emplace(fd,args);
}

bool Safe_queue::isEmpty(){
    unique_lock<mutex> l(sq_m);
    return que.empty();
}

void Safe_queue::processEvent(){
    
    unique_lock<mutex> l(sq_m);
    //如果队列空了的话，那么他继续睡觉，不去唤醒
   // sq_cv.wait(l,[this](){return !isEmpty();});
    this->sq_cv.wait(l);
    fun_cb fun = que.front();
    std::cout<<"调用了fun函数"<<std::endl;
    que.pop();
    auto it = client_fd.begin();
    fun(it->first,it->second);
    
    client_fd.erase(it->first);
    
}
