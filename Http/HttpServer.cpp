#include "HttpServer.h"
#include "TcpClient.h"
#include "Logger.h"
#include "Buffer.h"
#include <gflags/gflags.h>

DEFINE_int32(threadNum,2, "set thread num");

using namespace std;

void HttpServer::ReadCallback(Pine::clientPtr t,Buffer* inputBuffer){
    LOG_INFO("%s:%d<%s>",__FILE__,__LINE__,__FUNCTION__);
    string str = inputBuffer->getAllString();
    // LOG_INFO("%s",str.c_str());
    httpRequest_.request(t.get(),str);
}

void HttpServer::run(){
    tcpServer_.setThreadNum(FLAGS_threadNum);
    tcpServer_.setClientReadCallback(std::bind(&HttpServer::ReadCallback,this,std::placeholders::_1,std::placeholders::_2));
    tcpServer_.start();
    loop_.loop();
}