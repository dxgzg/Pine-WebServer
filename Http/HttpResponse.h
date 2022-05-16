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

// 同时也负责数据的写入

class HttpResponse
{
private:
    // 将新来的留言写入
//    void writeNewMsgToFile(rapidjson::Document& dom);


public:
    HttpResponse();
    ~HttpResponse();

    void SendFile(TcpClient* t,std::unique_ptr<HttpInfo>&);

    void reset();
    // 设置回复的字段，给业务逻辑代码调用的
    void setRespData(std::string& );

private:
    // todo post error的默认文件
    // 初始化头文件
    void initHttpResponseHead(HTTP_STATUS_CODE);
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
    // 发送头文件
    void sendResponseHeader(TcpClient* client);
private:
    std::unique_ptr<ResponseHead> responseHead_;
    std::string respData_;

};

