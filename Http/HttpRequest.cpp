#include "HttpRequest.h"
#include "TcpClient.h"
#include "Logger.h"
#include "HttpContent.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sstream>
#include <regex>
#include <iostream>
using namespace std;
std::map<std::string,std::string> HttpRequest::contentTypes = {
    {"js","application/x-javascript"},
    {"css","text/"},
    {"png","image/"},
    {"jpg","image/"},
    {"tar","application/"},
    {"zip","application/"},
    {"html","text/"},
    {"json","application/"}
};

// 设置请求的方式
void HttpRequest::setMethod(const std::string& method){
    if(method == "GET"){
        method_ = METHOD::GET;
    }
    else if(method == "POST"){
        method_ = METHOD::POST;
    }
    LOG_INFO("请求的方式为:%s",method.c_str());
}

void HttpRequest::setResponseFile(std::string& requestFile,
                                     HttpContent& content){
    if (requestFile == "/")
    { // 如果是/的话就给默认值
        content.filePath_ = path_;
        content.filePath_ += "/run.html";
        requestFile = "run.html";
    }
    else
    {
        content.filePath_ = path_;
        addFilePath(requestFile,content);
        // 利用open函数    
    }
    LOG_INFO("filePath: %s",content.filePath_.c_str());
}

bool HttpRequest::analyseFile(const string& request,HttpContent& content)
{
    regex reg(pattern_);
    smatch mas;
    regex_search(request,mas,reg);
    // 因为下标0是代表匹配的整体
    if(mas.size() < 3){
        LOG_ERROR("不是正常请求");
        // 啥都不是直接返回false
        return false;
    }
    //请求文件类型
    string method = mas[1];
    setMethod(method);

    // 请求的具体文件
    string requestFile = mas[2];

    /*
    * POST传数据，可接受到JSON文件，GET的话会转码
    */
    if(method_ == METHOD::POST){
        // 可以解析下是不是传过来json文件了
        std::stringstream ss(request);
        string tmp , leaveMsg;
        while(getline(ss,tmp)){
            leaveMsg = std::move(tmp);
        }
        requestFile = "/msg.json";
        httpResponse_.addNewMsg(leaveMsg);
    }

    // 先获取请求的文件
    setResponseFile(requestFile,content);

    bool flag = fileIsExist(content);
    // 如果文件不存在的话也就不需要解析类型
    if(!flag){
        LOG_INFO("未找到客户要的文件%s",content.filePath_.c_str());
        return false;
    }
    ::fstat(content.fileFd_,&content.fileStat_);
    // 解析文件类型
    flag = analyseFileType(requestFile,content);
    return flag;
}


void HttpRequest::request(TcpClient* client,std::string& msg){
    HttpContent content;
    bool flag = analyseFile(msg,content);
    cout << "flag状态是" << (flag? 1 : 0) << endl;
    Header(flag,content);
    if(!flag){
        httpResponse_.SendFile(client,false,content);
        client->CloseCallback();
        return ;
    }
    // 这个修改头文件的，先调用这个
    processHead(content);
    //这是文件找到了发送的
    httpResponse_.SendFile(client,true,content);

    // 关闭文件套接字
    close(content.fileFd_);
    client->CloseCallback();
}

bool HttpRequest::fileIsExist(HttpContent& content){
    content.fileFd_ = ::open(content.filePath_.c_str(),O_CLOEXEC | O_RDONLY);
    if (content.fileFd_ < 0)
    {   // 说明为找到请求的文件
        return false;
    }
    return true;
}

void HttpRequest::addFilePath(const std::string& requestFile,HttpContent& content){
    content.filePath_ += requestFile;
}

bool HttpRequest::analyseFileType(const std::string& requestFile,HttpContent& content){
     size_t i = requestFile.find_last_of(".");	
     if(i == string::npos)return false;
     content.fileType_ = requestFile.substr(i + 1);
     return true;
}

void HttpRequest::Header(bool flag,HttpContent& content)
{
    // 判断要发送的头部 true 表示200 false 表示404
    if(flag == true)
    { 
        content.header_ = "HTTP/1.1 200 OK\r\nConnection:keep-alive\r\n";
    }
    else
    {
        content.header_ = "HTTP/1.1 404 NOTFOUND\r\nContent-Length:0\r\n\r\n";
    }
}


void HttpRequest::processHead(HttpContent& content)
{
    string ContentType = "Content-Type:";
    if(content.fileType_ == "js"){
        ContentType += contentTypes["js"];
    }
    else{
        ContentType += (contentTypes[content.fileType_] + content.fileType_);
    }
    addHeader(ContentType,content);

    // 代完善，要打开文件 filePath_是请求文件的路径
    content.fileSize_= content.fileStat_.st_size;
    string ContentLength = "Content-Length:" + to_string(content.fileSize_);
    addHeader(ContentLength,content);
    // 最后加了一个结尾
    addHeader("",content);
}


void HttpRequest::addHeader(const string& head,HttpContent& content)
{
    if (!head.empty())
    {
        content.header_ += head;
        content.header_ += "\r\n";
    }
    // 自动加个结尾
    else
    {
        content.header_ += "\r\n";
    }
}
