#pragma once
#include <string>
#include <unordered_map>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "TcpClientCallback.h"
#include "TcpServer.h"
#include "EventLoop.h"
#include "HttpRequest.h"
class TcpClient;
class Buffer;
struct HttpContent;
class HttpRequest;
class HttpServer{
        // 文件的根目录
        EventLoop loop_;
        TcpServer tcpServer_;
        HttpRequest httpRequest_; // 会coredump
public:
        HttpServer():loop_(),tcpServer_(&loop_),httpRequest_(){}
        // 用户自定义的回调函数要正确的处理异常和自己负责关闭套接字
        void ReadCallback(Pine::clientPtr t,Buffer* inputBuffer);
        void run();
};