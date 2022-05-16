//
// Created by dxgzg on 14/05/22.
//

#ifndef PINE_HEADER_H
#define PINE_HEADER_H
#include <map>
#include <string>
#include <memory>
#include "const.h"

struct RequestFileInfo;
struct Header{
public:
    std::map<std::string,std::string> kv_; // 存储
    std::map<std::string ,std::string> queryData_;// 存放url的参数
    std::string bodyData_; // 存放body,例如json
    std::string headerTmp_; // 头文件没接收完整，不存body，body放在jsonData中
    std::string bodyTmp_; // body 的暂存
    std::string requestURI; // 请求的路径
    std::unique_ptr<RequestFileInfo> reqFileInfo_; // 解析的文件信息
    std::string method_;
    PARSE_STATUS status_;// 解析的状态
    HTTP_STATUS_CODE code_;

    Header();
    ~Header() = default;
};
#endif //PINE_HEADER_H
