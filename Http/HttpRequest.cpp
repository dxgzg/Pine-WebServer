#include "HttpRequest.h"
#include "TcpClient.h"
#include "Logger.h"
#include "HttpParse.h"
#include "HttpResponse.h"

using namespace std;
bool HttpRequest::analyseFile(TcpClient* client,std::string& msg,unique_ptr<HttpInfo>& httpInfo,postCallback& cb){
    return httpInfo->parse_->analyseFile(client,msg,cb);
}
// 只有解析完成才能进入的
bool HttpRequest::request(TcpClient* client,unique_ptr<HttpInfo>& httpInfo){
    httpInfo->response_->initHttpResponseHead(client->getHttpStatusCode());

    if(client->getHttpStatusCode() == HTTP_STATUS_CODE::NOT_FOUND){ // 为POST方法和404状态码加一个协议结尾
        // post和404不支持长链接
        string connect = "Connection: close";
        httpInfo->response_->addHeader(connect);

        string s = "";
        httpInfo->response_->addHeader(s); // 加一个结尾
        return false;
    } else if(httpInfo->parse_->getMethod() == METHOD::POST){

        string connect = "Connection:keep-alive";
        httpInfo->response_->addHeader(connect);

        string ContentType = "Content-Type:application/json";
        httpInfo->response_->addHeader(ContentType);

        httpInfo->response_->addHeader("Content-Length:11");

        httpInfo->response_->addHeader(""); // 加一个结尾
        return true;
    } else if(httpInfo->parse_->getMethod() == METHOD::GET){
        // 这个修改头文件的，先调用这个,长链接也是在这里加
        httpInfo->response_->processHead(httpInfo->parse_);
    }

    return true;    
}



HttpRequest::~HttpRequest() = default;