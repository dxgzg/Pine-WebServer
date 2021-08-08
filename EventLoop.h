#pragma once
#include <vector>
#include <memory>
#include <functional>
#include <thread>
#include <mutex>


#include "Epoller.h"

class Channel;
class HeadDetection;

class EventLoop{
    using Function = std::function<void()>;
public:
    EventLoop();
    void loop();
    void updateChannel(Channel* channel);
    void runInLoop(const Function& cb);

	// 把cb放入队列中，唤醒loop所在的线程，执行cb
	void queueInLoop(const Function& cb);
	// 唤醒loop所在的线程的
	void wakeup();
    void handleRead();
    void doPendingFunctor();
    
    std::thread::id getThreadId()const{return threadId_;}

    HeadDetection* getHeadDetection(){return headDet_.get();}

    // void addHeadDetection(int fd,std::function<void()> callback);
    ~EventLoop();
private:
    std::unique_ptr<Epoller> epoller_;
    int wakefd_;
    std::vector<Function> pendingFunctors_;
    std::thread::id threadId_;
    mutable std::mutex mutex_;
    std::unique_ptr<Channel> wakeChanel_;
    std::unique_ptr<HeadDetection> headDet_;
};