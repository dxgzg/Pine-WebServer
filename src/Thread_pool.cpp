#include"Thread_pool.h"
#include"Safe_queue.h"
#include <iostream>
using namespace std;
using std::thread;
Thread_pool::Thread_pool(std::shared_ptr<Safe_queue> que){
    this->sf_que = que;
    cout<<"thread pll sf_que use "<<sf_que.use_count()<<endl;
    for(int i = 0;i < this->thread_num;++i){
        thread t(&Thread_pool::worker,this);
        thread_pool.emplace_back(std::move(t));
    }
}

void Thread_pool::worker(){
    while(thread_run_flag){
        sf_que->processEvent();
    }
}