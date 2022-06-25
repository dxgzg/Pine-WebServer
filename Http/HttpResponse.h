#pragma once
#include <cstdio>
#include <string>
#include <memory>
#include <functional>
// 第三方库—JSON
#include "document.h"
#include "const.h"

class TcpClient;
struct ResponseHead;
struct RequestFileInfo;
struct HttpParse;
class HttpInfo;
class Header;

class HttpResponse
{
private:

//    void writeNewMsgToFile(rapidjson::Document& dom);


public:
    HttpResponse();
    ~HttpResponse();

    void SendFile(TcpClient* t,std::unique_ptr<HttpInfo>&);

    // todo 换成智能指针resst
    void reset();
    // 设置回复的字段，给业务逻辑代码调用的
    void setResponseData(HTTP_STATUS_CODE,std::string);

    // todo 设置cookie,给业务使用者调用; cookie ex:id=name
    void setCookie(const char* cookie,const char* path,int maxAge,bool httpOnly= true);

private:
    // todo post error的默认文件
    // 初始化头文件
    void initHttpResponseHead();
    // 添加\r\n
    void addHeaderEnd();
    // 设置keep-alive
    void setConnection(Header* header);
    // 设置Content-Length todo 未来要改成多态
    void setContentLength(Header* header);
    // 设置Content-Type todo 未来要改成多态
    void setContentType(Header* header);

    // 设置响应头文件
    void setHeaderResponse(Header* header);
    // 添加响应k-v
    void addHttpResponseHead(std::string k,std::string v);
    // 设置cookie
    void setCookie(Header* header);
    // 发送头文件
    void sendResponseHeader(TcpClient* client);
private:
    std::unique_ptr<ResponseHead> responseHead_;
    std::string respData_;
    std::string cookie_;
};

