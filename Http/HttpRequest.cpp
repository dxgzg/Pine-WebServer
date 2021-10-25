#include "HttpRequest.h"
#include "TcpClient.h"
#include "Logger.h"
#include "HttpParse.h"
#include "HttpResponse.h"

using namespace std;

bool HttpRequest::request(TcpClient* client,std::string& msg,unique_ptr<HttpInfo>& httpInfo){

    bool flag = httpInfo->parse_->analyseFile(msg);
    httpInfo->response_->initHttpResponseHead(flag);
    if(!flag){
        return false;
    }

    // 这个修改头文件的，先调用这个
    httpInfo->response_->processHead(httpInfo->parse_);
    
    return true;
}



HttpRequest::~HttpRequest() = default;