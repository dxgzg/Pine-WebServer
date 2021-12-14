#pragma once
#include <cstdio>
#include <string>
#include <memory>
#include <functional>
// 第三方库—JSON
#include "document.h"
#include "const.h"

class TcpClient;
class ResponseHead;
struct RequestFileInfo;
struct HttpParse;
class HttpInfo;

// 同时也负责数据的写入

class HttpResponse
{
private:
    // 将新来的留言写入
    void writeNewMsgToFile(rapidjson::Document& dom);
public:
    HttpResponse();
    ~HttpResponse();

    void SendFile(TcpClient* t,bool isRequestOk,std::unique_ptr<HttpInfo>&);

    void initHttpResponseHead(HTTP_STATUS_CODE);
    void addHttpResponseHead(const std::string& head);
    // 把一些头文件的信息都加进来，只有成功的时候调用这个函数,
    // 并返回文件中的数据
    void processHead(std::unique_ptr<HttpParse>& content);
    void addHeader(const std::string& head);
    void reset();
private:
private:
    std::unique_ptr<ResponseHead> responseHead_;
};

