#pragma once
#include <string>
#include <memory>
#include "HttpResponse.h"
#include "nocopyable.h"

class TcpClient;
class HttpInfo;

class HttpRequest : public nocopyable
{
public:
    HttpRequest() = default;
    ~HttpRequest();

    bool analyseFile(std::string& msg,std::unique_ptr<HttpInfo>& httpInfo);
private:
};


