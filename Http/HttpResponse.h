#pragma once
#include <cstdio>
#include <string>
#include <mutex>
#include <atomic>
// 第三方库—JSON
#include "document.h"

class TcpClient;
struct HttpContent;



// 同时也负责数据的写入

class HttpResponse
{
private:
    // 将新来的留言写入
    void writeNewMsgToFile(rapidjson::Document& dom);


public:
    HttpResponse();
    ~HttpResponse() = default;

    void SendFile(TcpClient* t,bool isRequestOk,HttpContent& content);
    bool initFile(rapidjson::Document& d);
    
    bool addNewMsg(std::string&);

private:
    std::atomic<int> id_; // 回头这个要改
    // std::mutex 
};

