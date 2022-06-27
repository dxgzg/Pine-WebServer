#pragma once
#ifndef PINE_CONST_H
#define PINE_CONST_H
#include <map>
#include <string>
#include <set>
#include <functional>

static std::set<std::string> METHOD{
        {"POST"},
        {"GET"},
        {"NONE"}
};

enum class PARSE_STATUS{
    PARSE_NONE = 0,
    PARSE_ERROR,
    PARSE_HEADER_CONTINUE,
    PARSE_BODY_CONTINUE,
    PARSE_OK,
};

enum class HTTP_STATUS_CODE{
    NONE = 0,
    OK = 200,
    NOT_FOUND = 404,
};

enum class CLIENT_STATUS{
    NONE = 0,
    CONNECT,
    SEND_CONTINUE,
    WAIT_DISCONNECT,
    MUST_CLOSE_CONNECT,
    DISCONNECT
};
// std::map<std::string,std::string> httpContentTypes = {
//     {"js","application/x-javascript"},
//     {"css","text/"},
//     {"png","image/"},
//     {"jpg","image/"},
//     {"tar","application/"},
//     {"zip","application/"},
//     {"html","text/"},
//     {"json","application/"}
// };
using timerCallback = std::function<void()>;
#endif