#include "HttpResponse.h"
#include "writer.h"
#include "stringbuffer.h"
#include "filewritestream.h"
#include "filereadstream.h"
#include "prettywriter.h"
#include "HttpParse.h"
#include <sys/sendfile.h>
#include <gflags/gflags.h>
using namespace std;

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


void HttpResponse::SendFile(TcpClient* client,bool isRequestOk,unique_ptr<RequestFileInfo>& reqFileInfo)
{
    // 发送http头
    size_t len = client->send(responseHead_->responseHeader_);
    LOG_INFO("len header%zu\n",len);
    LOG_INFO("文件名字是:%s",reqFileInfo->fileName_.c_str());

    // 发完了头，在发请求文件的信息。如果是404这里是没有的
    if (isRequestOk == true)
    {
        char* buff = (char*)malloc(reqFileInfo->fileSize_);
        ::read(reqFileInfo->fileFd_,buff,reqFileInfo->fileSize_);
        string s(buff,reqFileInfo->fileSize_); // 性能会损失，但是不需要判断二进制了
        client->send(std::move(s));
        free(buff);    
    } 

    // 发送完文件关闭套接字
    close(reqFileInfo->fileFd_);
    // client->CloseCallback();
}

bool HttpResponse::initFile(Document& dom){
    FILE* fp = fopen("./www/dxgzg_src/msg.json","r+");
    if(fp == NULL)return false;
    char readBuffer[65536];
    FileReadStream is(fp, readBuffer, sizeof(readBuffer));
    dom.ParseStream(is);
    fclose(fp);
    return true;
}


void HttpResponse::writeNewMsgToFile(Document& dom){
    FILE* fp = fopen("./www/dxgzg_src/msg.json","r+");
    char writeBuffer[65536];
    FileWriteStream os(fp, writeBuffer, sizeof(writeBuffer));
    PrettyWriter<FileWriteStream> writer(os);
    dom.Accept(writer);
    fflush(fp);
    fclose(fp);
}


bool HttpResponse::addNewMsg(std::string& msg){
    Document dom;
    initFile(dom);
    size_t start = msg.find_first_of(":");
    if(start == std::string::npos){
        return false;
    }
    // 解析新添加的留言
    size_t end = msg.find_last_of("}");
    std::string content = msg.substr(start + 2,end - start - 3);
    LOG_INFO("新增一条留言:%s",content.c_str());
    Document::AllocatorType&  allocator = dom.GetAllocator();
    Value item(Type::kObjectType);
    // item.AddMember("id",id_++,allocator);
    Value s;
    s.SetString(StringRef(content.c_str()));
    item.AddMember("content", s, allocator);
    if(dom.HasMember("LeavingMsg")){
        // cout << "have LeavingMsg" << endl;
    }
    else{
        // cout << "do not have LeavingMsg" << endl;
        return false;
    }

    dom["LeavingMsg"].PushBack(item,allocator);
    writeNewMsgToFile(dom);
    return true;
}

void HttpResponse::addHttpResponseHead(const string& head){
    responseHead_->responseHeader_ += head;
}

void HttpResponse::processHead(unique_ptr<HttpParse>& httpParse)
{
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