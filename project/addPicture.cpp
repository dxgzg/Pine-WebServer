//
// Created by cy & dxgzg on 2022/5/29.
//
#include "ready.h"
#include "random.h"
#include "Base64.h"

#include <fstream>

using namespace rapidjson;
using namespace std;
void addPicture(HttpInfo* info){
    string args = info->getBodyData();
    Document tmp;
    tmp.Parse(args.c_str());

    // todo 鉴权

    if(tmp.HasParseError()){
        LOG_ERROR("parse args error");
        info->setResponse(HTTP_STATUS_CODE::NOT_FOUND,"parse error");
        // LOG_ERROR("parse args error args:%s",args.c_str());
        return ;
    }
    if(!tmp.HasMember("content")){
        LOG_ERROR("不含有content");
        info->setResponse(HTTP_STATUS_CODE::NOT_FOUND,"not have content");
        return ;
    }

    string str = tmp["content"].GetString();
    // cout << str << endl;
    string s(str.data(),30);

    int index = s.find_first_of(",");
    s = s.substr(0,index);
    int start = s.find_first_of("/");
    int end = s.find_first_of(";");

    string pictureFmt = s.substr(start + 1,end - start - 1);
    LOG_INFO("picture fmt:%s",pictureFmt.c_str());
    string randomName = getName(16);
    string picname = "./www/dxgzg_src/img/";
    picname += randomName;
    picname += ".";
    picname += pictureFmt;
    LOG_INFO("file name:%s",picname.c_str());

    string content(str.data() + index + 1);
    string imgdecode64 = base64_decode(content);

    ofstream os(picname,ios::out | ios::binary);
    if(!os || !os.is_open() || os.bad() || os.fail()){
        cout << "file open error" << endl;
        return ;
    }
    os << imgdecode64;
    os.close();


    FILE* fp = fopen("./www/dxgzg_src/pictureList.json","rw");
    char buffer[65536];
    FileReadStream is(fp,buffer,sizeof(buffer));
    string listName = "img/";
    listName += randomName ;
    listName += ".";
    listName += pictureFmt;

    tmp.ParseStream(is);
    Value val(listName.c_str(), tmp.GetAllocator());
    tmp["pictureList"].PushBack(val,tmp.GetAllocator());
    fclose(fp);

    char writerbuffer[65536];
    fp = fopen("./www/dxgzg_src/pictureList.json","wr");
    FileWriteStream os2(fp,writerbuffer,sizeof(writerbuffer));

    Writer<FileWriteStream> writer(os2);
    tmp.Accept(writer);

    fclose(fp);
    info->setResponse(HTTP_STATUS_CODE::OK,"add picture successful");
    return ;
}