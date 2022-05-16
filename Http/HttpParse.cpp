#include "HttpParse.h"
#include "HttpRequest.h"
#include "HttpResponse.h"
#include "const.h"
#include "Logger.h"
#include "TimeStamp.h"
#include "Header.h"
#include "random.h"

#include <regex>
#include <gflags/gflags.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unordered_set>

using namespace std;


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

bool RequestFileInfo::getFileStat(){
    fileFd_ = ::open(filePath_.c_str(),O_CLOEXEC | O_RDONLY);
    bool flag = true;
    if (fileFd_ < 0)
    {
        // 说明未找到请求的文件
        LOG_ERROR("file not exist:%s",filePath_.c_str());
        fileType_ = "html";// 校正一下html
        // todo 可以加载个默认404页面
        filePath_ = FLAGS_path +"/404.html";
        fileFd_ = ::open(filePath_.c_str(),O_CLOEXEC | O_RDONLY); // 404.html必存在
        flag = false;
    }
    ::fstat(fileFd_,&fileStat_);
    fileSize_ = fileStat_.st_size;
    LOG_INFO("request file exist：%s",filePath_.c_str());
    return flag;
 }

void ResponseHead::initHttpResponseHead(HTTP_STATUS_CODE code){
    switch (code)
    {
    case HTTP_STATUS_CODE::OK:
        responseHeader_ = "HTTP/1.1 200 OK\r\n";//Origin:*\r\n
        break;
    case HTTP_STATUS_CODE::NOT_FOUND:
        responseHeader_ = "HTTP/1.1 404 NOTFOUND\r\n";
        break;
    default: 
        break;
    }
}

HttpInfo::HttpInfo():response_(make_unique<HttpResponse>())
                    ,request_(make_unique<HttpRequest>())
                    ,parse_(make_unique<HttpParse>()){}

HttpParse::HttpParse():path_(FLAGS_path),
                        header_(make_unique<Header>()){}

void HttpParse::setError(){
    header_->status_ = PARSE_STATUS::PARSE_ERROR;
    header_->code_ = HTTP_STATUS_CODE::NOT_FOUND;
}

bool HttpParse::simpleFilter(std::string& s){
    auto npos = std::string::npos;
    for(auto& val : filterSet){
        if(s.find(val) != npos)return false;
    }
    return true;
}

bool HttpParse::parseRequestLine(const std::string& oneData,size_t oneIndex){
    size_t index1 = oneData.find(" ");
    size_t index2 = oneData.find(" ",index1 + 1);
    if(index1 == string::npos || index2 == string::npos){
        LOG_ERROR("http header error:%s",oneData.c_str());
        return false;
    }

    string method = oneData.substr(0,index1);

    // 判断请求方式
    bool ans = parseMethod(method);
    if(!ans){
        LOG_ERROR("parse method error%s",method.c_str());
        return false;
    }

    // 解析url中携带的参数
    string path = oneData.substr(index1 + 1,index2 - index1 - 1);
    size_t flagSpilt = path.find("?"); // 找到?
    if(flagSpilt != string::npos){
        header_->requestURI = path.substr(0,flagSpilt);

        string query = path.substr(flagSpilt + 1);
        size_t pos = 0;
        while(1){
            size_t endIndex = query.find("&",pos);
            auto p = spilt(query,"=",pos,endIndex,1);
            if(p.first == "" && p.second == "")break;
            header_->queryData_[p.first] = p.second;
        }
    } else{
        header_->requestURI = path;
    }
    
    //todo: 判断HTTP版本
//    string httpVersion = oneData.substr(index2 + 1,oneIndex - index2 - 1);

    return true;
}

void HttpParse::parseRequestKV(const std::string& request,size_t startIndex,size_t lastLineIndex){
    // 解析头文件各个字段属性
    size_t index = startIndex; // 偏移量,第一行的后面
    size_t endIndex = 0;

    while( index < lastLineIndex && ((endIndex = request.find("\r\n",index)) != string::npos)){
        auto p = spilt(request,":",index,endIndex,2);
        header_->kv_[p.first] = p.second;
    }
}


// todo 解决粘包的问题
bool HttpParse::analyseFile(const string& request){
    if(header_->status_ == PARSE_STATUS::PARSE_NONE){
        // 解析HTTP第一行数据
        size_t oneIndex = request.find("\r\n");
        string oneData = request.substr(0, oneIndex);
        bool flag = parseRequestLine(oneData,oneIndex);
        if(!flag){
            setError();
            return false;
        }
        // 判断HTTP头文件是否完整
        size_t lastLineIndex = request.find("\r\n\r\n");
        if(lastLineIndex == string::npos){
            setError();
            return false;
        }

        // 解析HTTP头文件的各个属性
        parseRequestKV(request,oneIndex + 2,lastLineIndex);// \r\n占两个字节

        // 解析body数据并且判断是否接收完成
        flag = parseBody(request,lastLineIndex);
        if(!flag){
            header_->status_ = PARSE_STATUS::PARSE_BODY_CONTINUE;
            return false;
        }

        setParseOK();
    } else if(header_->status_ == PARSE_STATUS::PARSE_BODY_CONTINUE){
        // todo 这里可能会遇到>的情况，就是粘包，后期需要改善
        int cLength = stoi(header_->kv_["Content-Length"]);
        if(header_->bodyTmp_.size() + request.size() == cLength){
            setParseOK();
            header_->bodyData_ = move(header_->bodyTmp_) + move(request);
        } else{
            header_->bodyTmp_ += move(request);
            return false;
        }
    }

    LOG_INFO("parse ok method:%s uri:%s jsonData:%s",header_->method_.c_str(),
             header_->requestURI.c_str(),header_->bodyData_.c_str());

    // todo 设置请求文件
    if(header_->method_ == "GET"){
        bool flag = setResponseFile(header_->requestURI);
        if(!flag){
            setError();
            return false;
        }
    }
    LOG_INFO("request file exist");
    return true;
    //todo: 如果HTTP头文件缺少
//    size_t lastLineIndex = 0;
//    lastLineIndex = request.find("\r\n\r\n");
//    if(lastLineIndex == string::npos){
//        header_->status_ = PARSE_STATUS::PARSE_HEADER_CONTINUE;
//        header_->headerTmp_ += request;
//
//        LOG_ERROR("need more data");
//        return false;
//    }

}

void HttpParse::setParseOK(){
    header_->status_ = PARSE_STATUS::PARSE_OK;
    header_->code_ = HTTP_STATUS_CODE::OK;
}

bool HttpParse::parseBody(const std::string& request,size_t lastLineIndex){
    auto it = header_->kv_.find("Content-Length");
    if(it != header_->kv_.end()){
        size_t dataIndex = lastLineIndex + 4;
        string data = request.substr(dataIndex);

        int cLength = stoi(it->second);
        if(cLength != data.size()){
            header_->bodyTmp_.reserve(cLength);
            header_->bodyTmp_ += std::move(data);
            return false;
        } else{
            header_->bodyData_ = move(data);
        }
    }

    return true;
}

bool HttpParse::parseMethod(std::string& method){
    for(auto& c :method){
        toupper(c);
    }
    if(METHOD.find(method) == METHOD.end())return false;

    header_->method_ = method;
    return true;
}

// pos:偏移量注意是个引用,endIndex:参数结尾的下标.addPos:校正偏移量
pair<string,string> HttpParse::spilt(const string& s,string sep,size_t& pos,size_t endIndex,size_t addPos){
    size_t startIndex = s.find(sep,pos);
    if(startIndex == string::npos)return pair<string,string>();
    string key = s.substr(pos,startIndex - pos);

    string value ="";
    if(endIndex == string::npos){ // 比如 name=dxgzg,后面没有&就需要走第一个了。
        value =  s.substr(startIndex + 1);
        pos = s.size(); // endIndex + 1就等于0了，正数溢出
    } else{
        value = s.substr(startIndex + 1, endIndex - startIndex - 1);
        pos = endIndex + addPos;// ex: \r\n偏移量加2,&偏移量加1
    }

    pair<string,string> ans(key,value);
    return ans;
}

bool HttpParse::setResponseFile(std::string& requestFile){
    // 设置请求
    if (requestFile == "/")
    { // 如果是/的话就给默认值
        header_->reqFileInfo_->filePath_ = path_+ FLAGS_index;
        requestFile = FLAGS_index;
    }else{
        header_->reqFileInfo_->filePath_ = path_;
        header_->reqFileInfo_->filePath_ += requestFile; 
    }
    LOG_INFO("request file path :%s",header_->reqFileInfo_->filePath_.c_str());

    // 先看文件是否存在，这样方便设置404
    bool flag = header_->reqFileInfo_->getFileStat();
    if(!flag)return false;

    flag = analyseFileType(header_->reqFileInfo_->filePath_ );
    if(!flag)return false;



    return true;
}

bool HttpParse::analyseFileType(const std::string& requestFile){
     size_t i = requestFile.find_last_of(".");	
     if(i == string::npos) {
         LOG_ERROR("parse file type error:%s",requestFile.c_str());
         return false;
     }
     header_->reqFileInfo_->fileType_ = requestFile.substr(i + 1);
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
    this->header_->reqFileInfo_->reset();
    this->header_.reset(new Header());
}

void HttpResponse::reset(){
    responseHead_->responseHeader_ = "";
    respData_ = "";
}

void HttpInfo::reset(){
    if(!isParseFinish())return ;
    this->response_->reset();
    this->parse_->reset();
}

bool HttpInfo::isParseFinish(){
    if(parse_->getHeader()->status_ == PARSE_STATUS::PARSE_BODY_CONTINUE ||
       parse_->getHeader()->status_ == PARSE_STATUS::PARSE_HEADER_CONTINUE){
        return false;
    }
    return true;
}

RequestFileInfo::~RequestFileInfo() = default;
HttpParse::~HttpParse() = default;
HttpInfo::~HttpInfo() = default;
ResponseHead::~ResponseHead() = default;