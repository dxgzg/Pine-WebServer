#include "HttpParse.h"
#include "HttpRequest.h"
#include "HttpResponse.h"
#include "const.h"
#include "Logger.h"

#include <regex>
#include <gflags/gflags.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
using namespace std;

DEFINE_string(serverName,"Pine", "server name");

 bool RequestFileInfo::fileIsExist(){
    fileFd_ = ::open(filePath_.c_str(),O_CLOEXEC | O_RDONLY);
    if (fileFd_ < 0)
    {   // 说明未找到请求的文件
        return false;
    }
    return true;
 }

void ResponseHead::initHttpResponseHead(bool flag){
    if(flag){
        responseHeader_ = "HTTP/1.1 200 OK\r\nConnection:keep-alive\r\n";
    } else{
        responseHeader_ = "HTTP/1.1 404 NOTFOUND\r\nContent-Length:0\r\n";
    }
    responseHeader_ += "Server:" + FLAGS_serverName + "\r\n";
}

void ResponseHead::addResponseHead(const std::string& s){
    responseHeader_ += s;
}


HttpInfo::HttpInfo():response_(make_unique<HttpResponse>())
                    ,request_(make_unique<HttpRequest>())
                    ,parse_(make_unique<HttpParse>())
{

}

HttpParse::HttpParse():reqFileInfo_(make_unique<RequestFileInfo>()),method_(){

}

bool HttpParse::analyseFile(const string& request)
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
    * POST暂时不去重构
    */
    if(method_ == METHOD::POST){
        // 可以解析下是不是传过来json文件了
        // std::stringstream ss(request);
        // string tmp , leaveMsg;
        // while(getline(ss,tmp)){
        //     leaveMsg = std::move(tmp);
        // }
        // requestFile = "/msg.json";
        // httpResponse_.addNewMsg(leaveMsg);
    }

    // 先获取请求的文件
    setResponseFile(requestFile);

    bool flag = reqFileInfo_->fileIsExist();
    // 如果文件不存在的话也就不需要解析类型
    if(!flag){
        LOG_INFO("未找到客户要的文件%s",reqFileInfo_->filePath_.c_str());
        return false;
    }
    reqFileInfo_->fileName_ = requestFile;
    ::fstat(reqFileInfo_->fileFd_,&reqFileInfo_->fileStat_);
    // 解析文件类型
    flag = analyseFileType(requestFile);
    return flag;
}


// 设置请求的方式
void HttpParse::setMethod(const std::string& method){
    if(method == "GET"){
        method_ = METHOD::GET;
    }
    else if(method == "POST"){
        method_ = METHOD::POST;
    }
    LOG_INFO("请求的方式为:%s",method.c_str());
}

void HttpParse::setResponseFile(std::string& requestFile){
    if (requestFile == "/")
    { // 如果是/的话就给默认值
        reqFileInfo_->filePath_ = path_;
        reqFileInfo_->filePath_ += "/run.html";
        requestFile = "run.html";
    }
    else
    {
        reqFileInfo_->filePath_ = path_;
        reqFileInfo_->filePath_ += requestFile; 
    }
    LOG_INFO("filePath: %s",reqFileInfo_->filePath_.c_str());
}

bool HttpParse::analyseFileType(const std::string& requestFile){
     size_t i = requestFile.find_last_of(".");	
     if(i == string::npos)return false;
     reqFileInfo_->fileType_ = requestFile.substr(i + 1);
     return true;
}

RequestFileInfo::~RequestFileInfo() = default;
HttpParse::~HttpParse() = default;
HttpInfo::~HttpInfo() = default;
ResponseHead::~ResponseHead() = default;