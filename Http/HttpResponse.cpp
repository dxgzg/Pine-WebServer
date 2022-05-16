#include "HttpResponse.h"
#include "HttpParse.h"
#include "Header.h"
#include "HttpRequest.h"
#include "TimeStamp.h"
#include "HttpCallback.h"

#include <sys/sendfile.h>
#include <gflags/gflags.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#if 1

#include <chrono>
#include <iostream>

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
        {"jpeg", "image/jpeg"}
};


void HttpResponse::initHttpResponseHead(HTTP_STATUS_CODE code) {
    responseHead_->initHttpResponseHead(code);

    addHttpResponseHead("Server", FLAGS_serverName);
    addHttpResponseHead("Date", TimeStamp::getGMT());
}

HttpResponse::HttpResponse() : responseHead_(make_unique<ResponseHead>()), respData_("") {}

void HttpResponse::sendResponseHeader(TcpClient* client){
    if(!respData_.empty()){
        responseHead_->responseHeader_ += respData_;
    }
    LOG_INFO("send response http header:%s",responseHead_->responseHeader_.c_str());
    client->send(responseHead_->responseHeader_);
}


void HttpResponse::SendFile(TcpClient *client, std::unique_ptr<HttpInfo> &httpInfo) {
    // 如果还没有完成解析，就不发送response请求，继续等待消息。
    if (!httpInfo->isParseFinish())return;
    auto &header = httpInfo->parse_->getHeader();
    if (header->method_ == "POST") {
        //POST先执行回调函数在来发送数据，注意逻辑顺序
        auto cb = HttpCallback::getPostCB();
        cb(header->requestURI, header->bodyData_);
    }
    // 设置添加响应头文件
    setHeaderResponse(header.get());

    // 发送头文件
    sendResponseHeader(client);

    auto& reqFileInfo = header->reqFileInfo_;
    client->start = system_clock::now();
    char *buff = (char *) malloc(reqFileInfo->fileSize_);
    int n = ::read(reqFileInfo->fileFd_, buff, reqFileInfo->fileSize_);
    // LOG_INFO("read buff:%d",n);
    string s(buff, reqFileInfo->fileSize_); // 性能会损失，但是不需要判断二进制了
    // auto end = system_clock::now();
    // auto duration = duration_cast<microseconds>(end - client->start);
    // cout <<  "读取文件花费了"
    // << double(duration.count()) * microseconds::period::num / microseconds::period::den
    // << "秒" << endl;
    client->send(std::move(s));
    free(buff);

    // 发送完文件关闭套接字
    close(reqFileInfo->fileFd_);
    if (header->kv_.find("Connection") == header->kv_.end() || header->kv_["Connection"] == "close") {
        client->CloseCallback(); // 不是长连接需要关闭
    }
}



void HttpResponse::addHttpResponseHead(string k, string v) {
    responseHead_->responseHeader_ += k + ":" + v;
    addHeaderEnd();
}

void HttpResponse::setRespData(std::string &data) {
    respData_ += data;
}

void HttpResponse::addHeaderEnd() {
    string s = "\r\n";
    responseHead_->responseHeader_ += s;
}

// todo: 添加post响应头文件
bool setPostHeaderResponse(Header *header) {

}

void HttpResponse::setConnection(Header *header) {
    // 判断是否添加keep-alive
    if (header->kv_.find("Connection") != header->kv_.end() && header->kv_["Connection"] != "close"
        && header->code_ != HTTP_STATUS_CODE::NOT_FOUND) {
        addHttpResponseHead("Connection", "keep-alive");
    }
}

void HttpResponse::setContentLength(Header *header) {
    string key = "Content-Length";
    string value = "";
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
        value = httpContentTypes[header->reqFileInfo_->fileType_];// 没有的文件也不会走到这一步的
    } else if (header->method_ == "POST") {
        value = httpContentTypes["json"];
    }
    value += "; charset=utf-8"; // 设置utf-8类型
    addHttpResponseHead(key, value);
}

void HttpResponse::setHeaderResponse(Header *header) {
    // 初始化
    initHttpResponseHead(header->code_);
    // 是否长连接
    setConnection(header);
    // 回复的长度
    setContentLength(header);
    //回复的类型
    setContentType(header);

    // 最后加一个结尾
    addHeaderEnd();
}

HttpResponse::~HttpResponse() {}