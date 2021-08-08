#include "HttpResponse.h"
#include "HttpContent.h"
#include "writer.h"
#include "stringbuffer.h"
#include "filewritestream.h"
#include "filereadstream.h"
#include "prettywriter.h"
#include <sys/sendfile.h>
#include <gflags/gflags.h>
#include <iostream>
using namespace std;

using namespace rapidjson;

DEFINE_int32(id,0,"leaving message start id");
// 500KB
static constexpr int BUFFSIZE = 1024 * 500;


HttpResponse::HttpResponse():id_(FLAGS_id){}

void HttpResponse::SendFile(TcpClient* client,bool isRequestOk,HttpContent& content)
{
    // int clientFd = client->getFd();
    size_t len = 0;
    // 头部一定是有的。
    len += client->send(content.header_);
    LOG_INFO("len header%zu\n",len);
    cout << "发送的头文件是：" << content.header_ << endl;
    // 发完了头，在发请求文件的信息。如果是404这里是没有的
    if (isRequestOk == true)
    {
        len = 0;

        char* buff = (char*)malloc(BUFFSIZE);
        ::read(content.fileFd_,buff,BUFFSIZE);
        client->send(buff);
        free(buff);
    }

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
    item.AddMember("id",id_++,allocator);
    Value s;
    s.SetString(StringRef(content.c_str()));
    item.AddMember("content", s, allocator);
    if(dom.HasMember("LeavingMsg")){
        cout << "have LeavingMsg" << endl;
    }
    else{
        cout << "do not have LeavingMsg" << endl;
        return false;
    }

    dom["LeavingMsg"].PushBack(item,allocator);
    writeNewMsgToFile(dom);
    return true;
}