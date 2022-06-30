#include "HttpResponse.h"
#include "HttpParse.h"
#include "Header.h"
#include "HttpRequest.h"
#include "TimeStamp.h"
#include "HttpCallback.h"

#include <gflags/gflags.h>

#if 1

#include <chrono>


using namespace std::chrono;
using namespace std;
#endif

using namespace rapidjson;
DEFINE_string(serverName, "Pine", "server name");
std::map<std::string, std::string> httpContentTypes = {
        {"js",   "application/x-javascript"},
        {"css",  "text/css"},
        {"png",  "image/png"},
        {"jpg",  "image/jpg"},
        {"tar",  "application/tar"},
        {"zip",  "application/zip"},
        {"html", "text/html"},
        {"json", "application/json"},
        {"jpeg", "image/jpeg"},
        {"woff2","font/woff2"}
};


void HttpResponse::initHttpResponseHead() {
    // responseHead_->initHttpResponseHead(code);

    addHttpResponseHead("Server", FLAGS_serverName);
    addHttpResponseHead("Date", TimeStamp::getUTC());
}

HttpResponse::HttpResponse() : responseHead_(make_unique<ResponseHead>()), respData_(""),cookie_("") {}

void HttpResponse::sendResponseHeader(TcpClient* client){
    if(!respData_.empty()){
        responseHead_->responseHeader_ += respData_;
    }
    responseHead_->responseStatue_ += responseHead_->responseHeader_;
    LOG_INFO("send response http header:%s",responseHead_->responseStatue_.c_str());
    client->send(responseHead_->responseStatue_);
}

void HttpResponse::setCookie(const char* cookie,const char* path,int maxAge,bool httpOnly){
    cookie_ += ";";
    cookie_ +=  path;
    cookie_ +=  ";";
    cookie_ +=  to_string(maxAge);
    if(httpOnly){
        cookie_ += ";";
        cookie_ += "HttpOnly";
    }
}

void HttpResponse::SendFile(TcpClient *client, std::unique_ptr<HttpInfo> &httpInfo) {
    // 如果还没有完成解析，就不发送response请求，继续等待消息。
    if (!httpInfo->isParseFinish())return;
    auto &header = httpInfo->parse_->getHeader();
    if (header->method_ == "POST") {
        //POST先执行回调函数在来发送数据，注意逻辑顺序
        auto cb = HttpCallback::getPostCB(header->requestURI.c_str());
        if(cb == nullptr){
            // todo 设置不存在的回调函数处理
            LOG_ERROR("%s not callback fun",header->requestURI.c_str());
        } else{
            cb(httpInfo.get());
        }
    } else if(header->method_ == "GET"){
        // GET先给个200
        responseHead_->initHttpResponseHead(header->code_);
    }
    // 设置添加响应头文件
    setHeaderResponse(header.get());

    // 发送头文件
    sendResponseHeader(client);

    auto& reqFileInfo = header->reqFileInfo_;
    client->start = system_clock::now();
    char *buff = (char *) malloc(reqFileInfo->fileSize_);
    ::read(reqFileInfo->fileFd_, buff, reqFileInfo->fileSize_);
    // LOG_INFO("read buff:%d",n);
    string s(buff, reqFileInfo->fileSize_); // 性能会损失，但是不需要判断二进制了
    // auto end = system_clock::now();
    // auto duration = duration_cast<microseconds>(end - client->start);
    // cout <<  "读取文件花费了"
    // << double(duration.count()) * microseconds::period::num / microseconds::period::den
    // << "秒" << endl;
    int n = client->send(std::move(s));
    free(buff);
    // 发送完文件关闭套接字
    close(reqFileInfo->fileFd_);

    // 发送失败已经会调用closeCallback了
    if (n == -1 || header->kv_.find("Connection") == header->kv_.end() || header->kv_["Connection"].find("close") != string::npos) {
        client->CloseCallback(); // 不是长连接需要关闭
    }
}



void HttpResponse::addHttpResponseHead(string k, string v) {
    responseHead_->responseHeader_ += k + ":" + v;
    addHeaderEnd();
}

void HttpResponse::setResponseData(HTTP_STATUS_CODE code,std::string data) {
    // get post 都需要这个
    responseHead_->initHttpResponseHead(code);
    respData_ += data;
}

void HttpResponse::addHeaderEnd() {
    string s = "\r\n";
    responseHead_->responseHeader_ += s;
}

void HttpResponse::setConnection(Header *header) {
    // 判断是否添加keep-alive
    // todo 写个trim去掉多余的空格
    if (header->kv_.find("Connection") != header->kv_.end() && header->kv_["Connection"].find("close") == string::npos
        && header->code_ != HTTP_STATUS_CODE::NOT_FOUND) {
        addHttpResponseHead("Connection", "keep-alive");
    }
}

void HttpResponse::setContentLength(Header *header) {
    string key = "Content-Length";
    string value = "";
    // todo 不太对劲，需要改一下。
    if (header->method_ == "GET") {
        value = to_string(header->reqFileInfo_->fileSize_);
    } else if (header->method_ == "POST") {
        value = to_string(respData_.size());
    }
    addHttpResponseHead(key, value);
}

void HttpResponse::setContentType(Header *header) {
    string key = "Content-Type";
    string value = "";
    if (header->method_ == "GET") {
        // 找不到类型，在上层就处理掉了，变成404.html了
        value = httpContentTypes[header->reqFileInfo_->fileType_];
    } else if (header->method_ == "POST") {
        value = httpContentTypes["json"];
    }
    value += "; charset=utf-8"; // 设置utf-8类型
    addHttpResponseHead(key, value);
}

void HttpResponse::setCookie(Header* header){
    if(cookie_.empty())return ;
    string k = "Set-Cookie";
    addHttpResponseHead(k,cookie_);
}

void HttpResponse::setHeaderResponse(Header *header) {
    // todo 响应状态应由用户提供初始化
   initHttpResponseHead();
    // 是否长连接
    setConnection(header);
    // 回复的长度
    setContentLength(header);
    //回复的类型
    setContentType(header);
    // 是否需要设置cookie
    setCookie(header);
    // 最后加一个结尾
    addHeaderEnd();
}

HttpResponse::~HttpResponse() {}