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

struct RequestFileInfo
{   
    int fileFd_; // 文件fd
    std::string filePath_;// 具体文件的路径
    std::string fileType_;// 请求文件的类型    
    std::string fileName_; // 文件名字
    size_t fileSize_; // 文件大小
    struct stat fileStat_;

public:
    bool fileIsExist();
    ~RequestFileInfo();
};

struct ResponseHead{
    std::string responseHeader_;  // http头response
    enum METHOD method_;
    std::string serverName_; // 到时候改成flag
    // 后期还可以加个时间格式     

public:
    void initHttpResponseHead(bool flag);
    void addResponseHead(const std::string& s = "\r\n");   
    ~ResponseHead();
};

class HttpParse{
    using postCallback = std::function<void(std::string,std::string)>;
private:
    // 在[]的^是以什么什么开头，放在[]里面的是非的意思
    const std::string pattern_ = "^([A-Z]+) ([A-Za-z./1-9-?=]*)";
    const std::string path_;
private:
    std::unique_ptr<RequestFileInfo> reqFileInfo_; // 解析的文件信息
    METHOD method_; 
public:
    HttpParse();
    bool analyseFile(const std::string&,postCallback&);
    void setResponseFile(std::string&);
    bool analyseFileType(const std::string&);
    std::unique_ptr<RequestFileInfo>& getFileInfo(){return reqFileInfo_;}
    METHOD getMethod(){return method_;}
    ~HttpParse();
private:
    
    void setMethod(const std::string&);
    bool simpleFilter(std::string&);
};

struct HttpInfo{
    std::unique_ptr<HttpResponse> response_;
    std::unique_ptr<HttpRequest> request_;
    std::unique_ptr<HttpParse> parse_;
    HttpInfo();
    ~HttpInfo();
};