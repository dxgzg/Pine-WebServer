#pragma once
#include <string>
#include <unordered_map>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "TcpClientCallback.h"
#include "TcpServer.h"
#include "EventLoop.h"

class TcpClient;
class Buffer;

class HttpServer{
    EventLoop loop_;// 主loop
    TcpServer tcpServer_;
public:
        HttpServer():loop_(),tcpServer_(&loop_){}
        // 用户自定义的回调函数要正确的处理异常和自己负责关闭套接字
        void ReadCallback(Pine::clientPtr t,Buffer* inputBuffer);
        void run();
};