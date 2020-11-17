#ifndef SAFE_QUEUE_H_INCLUDED
#define SAFE_QUEUE_H_INCLUDED
#include <queue>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <memory>
#include <iostream>
#include <unordered_map>


class Safe_queue
{
    //重命名
private:
    using fun_cb = std::function<void(int,void*)>;
    //关于线程
private:
    std::mutex sq_m;//sq指的是Safe_queue
    std::condition_variable sq_cv;
    //关于数据结构
private:
    std::queue<fun_cb> que;
    std::unordered_map<int,void*> client_fd;
   
    
public:
    //static std::shared_ptr<Safe_queue> sf_que; 
    Safe_queue(/* args */) = default;
    //使用包扩展了
    template<typename F,typename ...Args>
    void addEvent(F&& fun,Args&& ...args){
        std::unique_lock<std::mutex> l(sq_m);
        std::cout<<"添加了fun函数"<<std::endl;
        que.emplace(fun);
        addFd(std::forward<Args>(args)...);//包扩展一下
        this->sq_cv.notify_one();//有事件发生通知线程
    
    }
    void addFd(int,void*);
    void processEvent();
    static std::shared_ptr<Safe_queue> getQue();
    bool isEmpty();
    ~Safe_queue() = default;
};

#endif