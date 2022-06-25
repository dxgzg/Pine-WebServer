//
// Created by cy & dxgzg on 2022/5/15.
//

#ifndef PINE_HTTPCALLBACK_H
#define PINE_HTTPCALLBACK_H
#include "nocopyable.h"
#include "functional"
#include <string>

class HttpInfo;
#include <unordered_map>

class HttpCallback : public  nocopyable{
public:
    using callback = std::function<void(HttpInfo*)>;
public:
    // todo 待修改接口类型
    static void setPostCB(const char* s,callback cb);
    static callback getPostCB(const char* s);

    static void setGetCB(const char* s,callback cb);
    static callback getGetCB(const char* s);
private:
    HttpCallback() = default;
    static std::unordered_map<std::string,callback> postCallBack_;
    static std::unordered_map<std::string,callback> getCallBack_;
};



#endif //PINE_HTTPCALLBACK_H
