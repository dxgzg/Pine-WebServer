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
    using callback = std::function<void(HttpInfo*)>;
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
    void setPostReadCallback(const char* s,callback cb){
        HttpCallback::setPostCB(s,cb);
    }
    ~HttpServer();

};
// todo 研究clion左移键子
// todo messages cmake 乱码
// todo gdb-server
// todo g++报错
// todo 压缩图片
// todo GET、POST，加载静态资源。