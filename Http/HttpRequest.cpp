#include "HttpRequest.h"
#include "TcpClient.h"
#include "Logger.h"
#include "HttpParse.h"
#include "HttpResponse.h"

using namespace std;

bool HttpRequest::request(TcpClient* client,std::string& msg,unique_ptr<HttpInfo>& httpInfo,postCallback& cb){

    bool flag = httpInfo->parse_->analyseFile(msg,cb);
    httpInfo->response_->initHttpResponseHead(flag);

    if(!flag || httpInfo->parse_->getMethod() == METHOD::POST){ // 为POST方法和404状态码加一个协议结尾
        // post和404不支持长链接
        string connect = "Connection: close";
        httpInfo->response_->addHeader(connect);

        string s = "";
        httpInfo->response_->addHeader(s); // 加一个结尾
        return false;
    } 

    // 这个修改头文件的，先调用这个,长链接也是在这里加
    httpInfo->response_->processHead(httpInfo->parse_);
    
    return true;
}



HttpRequest::~HttpRequest() = default;