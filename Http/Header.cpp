//
// Created by cy & dxgzg on 2022/5/15.
//
#include "Header.h"
#include "HttpParse.h"
#include <memory>
using namespace std;
Header::Header():kv_(),queryData_(),bodyData_(""),headerTmp_(""),bodyTmp_(""),
                requestURI(""),reqFileInfo_(make_unique<RequestFileInfo>()),method_(""),
                status_(PARSE_STATUS::PARSE_NONE),code_(HTTP_STATUS_CODE::NONE){}