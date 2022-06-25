//
// Created by cy & dxgzg on 2022/5/15.
//

#include "HttpCallback.h"
#include "HttpParse.h"
using callback = HttpCallback::callback;

void HttpCallback::setPostCB(const char* s,callback cb){
    postCallBack_[s] = std::move(cb);
}
callback HttpCallback::getPostCB(const char* s){
    if(postCallBack_.find(s) == postCallBack_.end()){
        return nullptr;
    }
    return postCallBack_[s];
}

void HttpCallback::setGetCB(const char* s,callback cb){
    getCallBack_[s] = std::move(cb);
}
callback HttpCallback::getGetCB(const char* s){
    if(getCallBack_.find(s) == getCallBack_.end()){
        return nullptr;
    }
    return getCallBack_[s];
}


std::unordered_map<std::string,HttpCallback::callback> HttpCallback::postCallBack_ = std::unordered_map<std::string,HttpCallback::callback>();
std::unordered_map<std::string,HttpCallback::callback> HttpCallback::getCallBack_ = std::unordered_map<std::string,HttpCallback::callback>();