#pragma once
#include <string>
#include <unordered_map>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <functional>
#include <memory>
#include "TcpClientCallback.h"
#include "TcpServer.h"
#include "EventLoop.h"
#include "HttpCallback.h"

class TcpClient;
class Buffer;
class HttpInfo;


class HttpServer{
private:
    using callback = std::function<bool(std::string,std::string)>;
private:
    EventLoop loop_;// 主loop
    TcpServer tcpServer_;
    callback postCallback_;
public:
    HttpServer();
    // 用户自定义的回调函数要正确的处理异常和自己负责关闭套接字
    void ReadCallback(Pine::clientPtr t,Buffer* inputBuffer);
    void run();
    // todo 待修改
    void setPostReadCallback(callback cb){
        HttpCallback::setPostCB(cb);
    }
    ~HttpServer();

};
// todo 研究clion左移键子
// todo messages cmake 乱码
// todo gdb-server
