//
// Created by cy & dxgzg on 2022/5/15.
//

#ifndef PINE_HTTPCALLBACK_H
#define PINE_HTTPCALLBACK_H
#include "nocopyable.h"
#include "functional"
#include "string"

class HttpCallback : public  nocopyable{
    using callback = std::function<bool(std::string,std::string)>;
public:
    // todo 待修改接口类型
    static void setPostCB(callback cb){ postCallback_ = std::move(cb);}
    static callback getPostCB(){
        return postCallback_;
    }
private:
    HttpCallback() = default;
    static callback postCallback_;
};



#endif //PINE_HTTPCALLBACK_H
