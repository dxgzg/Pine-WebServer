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
#include <unordered_set>
using namespace std;

DEFINE_string(serverName,"Pine", "server name");
DEFINE_string(index,"run.html", "web index html");
DEFINE_string(path,"./www/dxgzg_src", "html path");

static unordered_set<string> filterSet{
    "http",
    "\\",
    "password",
    "/",
    "args"
    "bin"
    "user"
};

bool RequestFileInfo::fileIsExist(){
    fileFd_ = ::open(filePath_.c_str(),O_CLOEXEC | O_RDONLY);
    LOG_INFO("file:%s fd:%d",filePath_.c_str(),fileFd_);
    if (fileFd_ < 0)
    {   // 说明未找到请求的文件
        return false;
    }
    return true;
 }

void ResponseHead::initHttpResponseHead(HTTP_STATUS_CODE code){
    switch (code)
    {
    case HTTP_STATUS_CODE::OK:
        responseHeader_ = "HTTP/1.1 200 OK\r\n";//Origin:*\r\n
        break;
    case HTTP_STATUS_CODE::NOT_FOUND:
        responseHeader_ = "HTTP/1.1 404 NOTFOUND\r\nContent-Length:0\r\n";
        break;
    default: 
        break;
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

HttpParse::HttpParse():path_(FLAGS_path),
                        reqFileInfo_(make_unique<RequestFileInfo>()),method_()
{

}
bool HttpParse::simpleFilter(std::string& s){
    auto npos = std::string::npos;
    for(auto& val : filterSet){
        if(s.find(val) != npos)return false;
    }
    return true;
}
bool HttpParse::analyseFile(TcpClient* client,const string& request,postCallback& cb)
{
    regex reg(pattern_);
    smatch mas;
    regex_search(request,mas,reg);


    // 因为下标0是代表匹配的整体
    if(mas.size() < 3 ){
        LOG_ERROR("不是正常请求");
        client->setParseStatus(PARSE_STATUS::PARSE_OK);
        client->setHttpStatusCode(HTTP_STATUS_CODE::NOT_FOUND);
        // 啥都不是直接返回false
        return false;
    }
    //请求文件类型
    string method = mas[1];
    setMethod(method);
 
    // 请求的具体文件
    string requestFile = mas[2];
    bool flag = true;
    /*
    * POST传数据，可接受到JSON文件，GET的话会转码
    * POST暂时不去重构
    */
    if(method_ == METHOD::POST){
        client->setParseStatus(PARSE_STATUS::PARSE_CONTINUE);
        
        
        size_t start = request.find_last_of("\r\n");
        if(start == string::npos){
            return false;
        }

        size_t index = request.find("\"}");
        if(index == string::npos){ // 说明还需要继续读post过来的数据     
            return false;
        }
        client->setParseStatus(PARSE_STATUS::PARSE_OK);
        string args = request.substr(start + 1);

        // if(!simpleFilter(args)){ // 没有通过过滤的话
        //     LOG_ERROR("not pass fiter");
        //     return false;
        // }
        flag = cb(requestFile,args);
    } else{
        client->setHttpStatusCode(HTTP_STATUS_CODE::NOT_FOUND);
        client->setParseStatus(PARSE_STATUS::PARSE_OK);// 直接默认GET请求是读完的了
        // 先获取请求的文件
        setResponseFile(requestFile);

        LOG_INFO("parse request file:%s",requestFile.c_str());

        flag = reqFileInfo_->fileIsExist();
        reqFileInfo_->fileName_ = requestFile;

        // 如果文件不存在的话也就不需要解析类型
        if(!flag){
            LOG_INFO("未找到客户要的文件%s",reqFileInfo_->filePath_.c_str());
            return flag;
        }

        ::fstat(reqFileInfo_->fileFd_,&reqFileInfo_->fileStat_);
        // 解析文件类型
        flag = analyseFileType(requestFile);    
    }

    if(client->getParseStatus() == PARSE_STATUS::PARSE_OK){// 解析完成要移动数据了
        LOG_INFO("entry");
        client->readOk(request.size());
        if(flag){
            client->setHttpStatusCode(HTTP_STATUS_CODE::OK);
        } else{
            client->setHttpStatusCode(HTTP_STATUS_CODE::NOT_FOUND);
        }
        return flag;
    } else{
        return false;
    }
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
        reqFileInfo_->filePath_ = path_+ FLAGS_index;
        requestFile = FLAGS_index;
    }
    else
    {
        reqFileInfo_->filePath_ = path_;
        reqFileInfo_->filePath_ += requestFile; 
    }
    LOG_INFO("filePath: %s",reqFileInfo_->filePath_.c_str());
    LOG_INFO("name: %s",requestFile.c_str());
}

bool HttpParse::analyseFileType(const std::string& requestFile){
     size_t i = requestFile.find_last_of(".");	
     if(i == string::npos)return false;
     reqFileInfo_->fileType_ = requestFile.substr(i + 1);
     return true;
}

void RequestFileInfo::reset(){
    this->fileFd_ = -1;
    this->fileName_ = "";
    this->filePath_ = "";
    this->fileSize_ = 0;
    this->fileType_ = "";
}

void HttpParse::reset(){
    this->reqFileInfo_->reset();
}

void HttpResponse::reset(){
    responseHead_->responseHeader_ = "";
}

void HttpInfo::reset(){
    this->response_->reset();
    this->parse_->reset();
}

RequestFileInfo::~RequestFileInfo() = default;
HttpParse::~HttpParse() = default;
HttpInfo::~HttpInfo() = default;
ResponseHead::~ResponseHead() = default;