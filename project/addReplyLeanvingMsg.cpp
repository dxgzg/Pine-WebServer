//
// Created by cy & dxgzg on 2022/5/29.
//
#include "ready.h"
using namespace std;
using namespace rapidjson;
void addReplyMsg(HttpInfo* info){
    string args = info->getBodyData();

    Document tmp;
    tmp.Parse(args.c_str());
    if(tmp.HasParseError()){
        LOG_ERROR("parse args error args:%s",args.c_str());
        info->setResponse(HTTP_STATUS_CODE::NOT_FOUND,"parse error");
        return ;
    }
    if(!tmp.HasMember("replyMsg") || !tmp.HasMember("index")){
        LOG_ERROR("不含有replyMsg");
        info->setResponse(HTTP_STATUS_CODE::NOT_FOUND,"not have replyMsg");
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
        info->setResponse(HTTP_STATUS_CODE::NOT_FOUND,"not have leavingMsg");
        return ;
    }
    cout << "it is ok" << endl;

    int index = tmp["index"].GetInt();
    string s = tmp["replyMsg"].GetString();
    Value value;

    // 添加MSG
    if(d["LeavingMsg"][index].HasMember("replyMsg")){
        s = s + '\n' +  d["LeavingMsg"][index]["replyMsg"].GetString();
        value.SetString(s.c_str(),s.size());
        d["LeavingMsg"][index]["replyMsg"] = value;
    } else{
        value.SetString(s.c_str(),s.size()); // 添加完member会释放value的值
        d["LeavingMsg"][index].AddMember("replyMsg",value,d.GetAllocator());
    }

    string now = TimeStamp::Now();

    // 添加replyTime
    if(d["LeavingMsg"][index].HasMember("replyTime")){
        now = now + '\n' + d["LeavingMsg"][index]["replyTime"].GetString();
        value.SetString(now.c_str(),now.size());
        d["LeavingMsg"][index]["replyTime"] = value;
    } else{
        value.SetString(now.c_str(),now.size());
        d["LeavingMsg"][index].AddMember("replyTime",value,d.GetAllocator());
    }

    char writerbuffer[65536];
    fp = fopen("./www/dxgzg_src/msg.json","wr");
    FileWriteStream os(fp,writerbuffer,sizeof(writerbuffer));

    Writer<FileWriteStream> writer(os);
    d.Accept(writer);

    fclose(fp);
    info->setResponse(HTTP_STATUS_CODE::OK,"add reply msg successful");
    return ;
}