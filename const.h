#pragma once
#include <map>
#include <string>
enum class METHOD{
    GET = 1000,
    POST,
};
enum class PARSE_STATUS{
    PARSE_NONE = 0,
    PARSE_CONTINUE,
    PARSE_OK,
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