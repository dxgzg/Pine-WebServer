//
// Created by cy & dxgzg on 2022/5/29.
//
#include "ready.h"

using namespace rapidjson;
using namespace std;

void addLeavingMsg(HttpInfo* info){
    string args = info->getBodyData();
    Document tmp;
    tmp.Parse(args.c_str());
    if(tmp.HasParseError()){
        LOG_ERROR("parse args error args:%s",args.c_str());
        info->setResponse(HTTP_STATUS_CODE::NOT_FOUND,"parse error");
        return ;
    }
    if(!tmp.HasMember("content")){
        LOG_ERROR("不含有content");
        info->setResponse(HTTP_STATUS_CODE::NOT_FOUND,"not have content");
        return ;
    }
    Document d;
    FILE* fp = fopen("./www/dxgzg_src/msg.json","r");
    char buffer[65536];
    FileReadStream is(fp,buffer,sizeof(buffer));

    d.ParseStream(is);
    fclose(fp);
    if(!d.HasMember("LeavingMsg")){
        cout << "不含有leavingMsg" << endl;
        return ;
    }

    Value key(kObjectType);
    Value value;
    const char* str = tmp["content"].GetString();
    value.SetString(str,strlen(str));
    cout << value.GetString() << endl;
    key.AddMember("content",value,d.GetAllocator());

    string now = TimeStamp::Now();
    value.SetString(now.c_str(),now.size());
    key.AddMember("time",value,d.GetAllocator());

    d["LeavingMsg"].PushBack(key,d.GetAllocator());


    char writerbuffer[65536];
    fp = fopen("./www/dxgzg_src/msg.json","wr");
    FileWriteStream os(fp,writerbuffer,sizeof(writerbuffer));

    Writer<FileWriteStream> writer(os);
    d.Accept(writer);

    fclose(fp);

    info->setResponse(HTTP_STATUS_CODE::OK,"add msg successful");
    return ;
}