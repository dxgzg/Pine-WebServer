#include "HttpRequest.h"
#include "TcpClient.h"
#include "Logger.h"
#include "HttpParse.h"
#include "HttpResponse.h"

using namespace std;
bool HttpRequest::analyseFile(std::string& msg,unique_ptr<HttpInfo>& httpInfo){
    return httpInfo->parse_->analyseFile(msg);
}

HttpRequest::~HttpRequest() = default;