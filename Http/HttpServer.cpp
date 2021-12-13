#include "HttpServer.h"
#include "HttpParse.h"
#include "HttpResponse.h"
#include "HttpRequest.h"
#include "TcpClient.h"
#include "Logger.h"
#include "HttpParse.h"
#include "Buffer.h"

#include <gflags/gflags.h>

DEFINE_int32(threadNum,4, "set thread num");

using namespace std;

void HttpServer::ReadCallback(Pine::clientPtr client,Buffer* inputBuffer){
    string str = inputBuffer->getAllString();
    LOG_INFO("%s",str.c_str());
    unique_ptr<HttpInfo>& httpInfo = client->resetHttpInfo();

    // 解析请求的内容
    bool flag = httpInfo->request_->request(client.get(),str,httpInfo,postCallback_);

    // 发送请求的文件
    httpInfo->response_->SendFile(client.get(),flag,httpInfo);    
}

void HttpServer::run(){
    tcpServer_.setThreadNum(FLAGS_threadNum);
    LOG_INFO("conf threadNum:%d",FLAGS_threadNum);
    tcpServer_.setClientReadCallback(std::bind(&HttpServer::ReadCallback,this,std::placeholders::_1,std::placeholders::_2));
    tcpServer_.start();
    loop_.loop();
}

HttpServer::~HttpServer() = default;
HttpServer::HttpServer():loop_(),tcpServer_(&loop_),postCallback_(){
}