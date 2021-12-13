#include "HttpResponse.h"
#include "HttpParse.h"
#include <sys/sendfile.h>
#include <gflags/gflags.h>
using namespace std;


#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

using namespace rapidjson;

std::map<std::string,std::string> httpContentTypes = {
    {"js","application/x-javascript"},
    {"css","text/"},
    {"png","image/"},
    {"jpg","image/"},
    {"tar","application/"},
    {"zip","application/"},
    {"html","text/"},
    {"json","application/"}
};

// 500KB
static constexpr int BUFFSIZE = 1024 * 500;

void HttpResponse::initHttpResponseHead(bool flag){
    responseHead_->initHttpResponseHead(flag);
}
HttpResponse::HttpResponse():responseHead_(make_unique<ResponseHead>()){}


void HttpResponse::SendFile(TcpClient* client,bool isRequestOk,std::unique_ptr<HttpInfo>& httpInfo)
{   
    if(client->getParseStatus() != PARSE_STATUS::PARSE_OK){
        return ;
    }

    unique_ptr<RequestFileInfo>& reqFileInfo = httpInfo->parse_->getFileInfo();
    // 发送http头
    size_t len = client->send(responseHead_->responseHeader_);
    LOG_INFO("head:%s",responseHead_->responseHeader_.c_str());

    // 发完了头，在发请求文件的信息。如果是404这里是没有的
    if (isRequestOk == true && reqFileInfo->fileFd_ != -1)
    {   
        LOG_INFO("ENTRY filefd:%d",reqFileInfo->fileFd_);
        char* buff = (char*)malloc(reqFileInfo->fileSize_);
        ::read(reqFileInfo->fileFd_,buff,reqFileInfo->fileSize_);
        string s(buff,reqFileInfo->fileSize_); // 性能会损失，但是不需要判断二进制了
        client->send(std::move(s));
        free(buff);    
        // 发送完文件关闭套接字
        close(reqFileInfo->fileFd_);

    } else if(httpInfo->parse_->getMethod() == METHOD::POST){
        LOG_INFO("POST");
        // client->CloseCallback();
        client->send("{\"error\":0}");
    }

}

void HttpResponse::addHttpResponseHead(const string& head){
    responseHead_->responseHeader_ += head;
}

void HttpResponse::processHead(unique_ptr<HttpParse>& httpParse)
{
    string connect = "Connection:keep-alive";
    addHeader(connect);

    string ContentType = "Content-Type:";
    string fileType = httpParse->getFileInfo()->fileType_; 
    if(fileType == "js"){
        ContentType += httpContentTypes["js"];
    }
    else{
        ContentType += (httpContentTypes[fileType] + fileType);
    }
    addHeader(ContentType);

    httpParse->getFileInfo()->fileSize_= httpParse->getFileInfo()->fileStat_.st_size;
    string ContentLength = "Content-Length:" + to_string(httpParse->getFileInfo()->fileSize_);
    addHeader(ContentLength);

    // 最后加了一个结尾
    addHeader("");
}


void HttpResponse::addHeader(const string& head)
{   
    string s = "\r\n";
    if (!head.empty())
    {   
        s = head + "\r\n";
    }
    this->addHttpResponseHead(s);
}

HttpResponse::~HttpResponse(){}