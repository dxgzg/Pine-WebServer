//
// Created by cy & dxgzg on 2022/5/29.
//
#include "ready.h"

DEFINE_string(passKey,"passKey", "post picture to album");
using namespace std;
using namespace rapidjson;
void checkPassword(HttpInfo* info){
    string args = info->getBodyData();

    Document tmp;
    tmp.Parse(args.c_str());
    if(tmp.HasParseError()){
        LOG_ERROR("parse args error args:%s",args.c_str());
        info->setResponse(HTTP_STATUS_CODE::NOT_FOUND,"parse password error");
        return ;
    }
    if(!tmp.HasMember("passKey")){
        LOG_ERROR("不含有passKey");
        info->setResponse(HTTP_STATUS_CODE::NOT_FOUND,"not have password");
        return ;
    }
    if(!tmp["passKey"].IsString()){
        LOG_ERROR("password not string");
        info->setResponse(HTTP_STATUS_CODE::NOT_FOUND,"password not string");
        return ;
    }

    string passKey = tmp["passKey"].GetString();
    if(passKey != FLAGS_passKey){
        cout << tmp["passKey"].GetString() << endl;
        LOG_ERROR("passKey error");
        info->setResponse(HTTP_STATUS_CODE::NOT_FOUND,"password invalid");
        return ;
    }

    info->setResponse(HTTP_STATUS_CODE::OK,"check successful");
}