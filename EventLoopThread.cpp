#include "EventLoopThread.h"
#include "EventLoop.h"

EventLoopThread::EventLoopThread():loop_(std::make_unique<EventLoop>()){}

 EventLoopThread::~EventLoopThread(){
     
 }