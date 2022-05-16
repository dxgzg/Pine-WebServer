#pragma once
#include <string>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <memory>
#include <gflags/gflags.h>
#include <unordered_map>

#include "TcpClient.h"
#include "Logger.h"
#include "const.h"

class HttpResponse;
class HttpRequest;
class Header;

struct RequestFileInfo
{   
    int fileFd_ = -1; // 文件fd
    std::string filePath_ = "";// 具体文件的路径
    std::string fileType_ = "";// 请求文件的类型    
    std::string fileName_ = ""; // 文件名字
    size_t fileSize_ = 0 ; // 文件大小
    struct stat fileStat_;

public:
    RequestFileInfo():fileFd_(-1),filePath_(""),fileType_(""),fileName_(""),
                      fileSize_(0),fileStat_(){}
    // 设置文件的状态
    bool getFileStat();
    void reset();
    ~RequestFileInfo();
};

struct ResponseHead{
    std::string responseHeader_;  // http头response
    std::string serverName_; // 到时候改成flag
    // TODO 还可以加个时间格式

public:
    void initHttpResponseHead(HTTP_STATUS_CODE code);
    ~ResponseHead();
};

class HttpParse{
private:
    // 在[]的^是以什么什么开头，放在[]里面的是非的意思
//    const std::string pattern_ = "^([A-Z]+) ([A-Za-z./0-9-?=+]*)";
    std::string path_;


    std::unique_ptr<Header> header_;
public:
    HttpParse();
    bool analyseFile(const std::string&);
    bool setResponseFile(std::string&);
    bool analyseFileType(const std::string&);
    std::pair<std::string,std::string> spilt(const std::string& s,std::string sep,size_t& pos,size_t endIndex,size_t addPos);
    // todo reset
    void reset();
    std::unique_ptr<Header>&  getHeader(){return header_;}

    ~HttpParse();
private:
    // 解析第一行
    bool parseRequestLine(const std::string&,size_t);
    // 判断请求方式
    bool parseMethod(std::string&);
    // todo 解析HTTP版本
//    bool parseHttpVersion(std::string&);
    // 将状态码设置为404,解析也失败了
    void setError();
    // 解析HTTP头的kv字段
    void parseRequestKV(const std::string&,size_t,size_t);
    // 解析body数据
    bool parseBody(const std::string&,size_t);
    // 设置解析完成
    void setParseOK();

    // todo 过滤器
    bool simpleFilter(std::string&);
};

struct HttpInfo{
    std::unique_ptr<HttpResponse> response_;
    std::unique_ptr<HttpRequest> request_;
    std::unique_ptr<HttpParse> parse_;
    HttpInfo();
    ~HttpInfo();
    void reset();
    bool isParseFinish();
};