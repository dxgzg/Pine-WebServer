#pragma once
#include <string>
#include "HttpResponse.h"
#include "nocopyable.h"
#include <map>
class TcpClient;
struct HttpContent;

class HttpRequest : public nocopyable
{
    enum class METHOD{
        GET = 0,
        POST = 1,
    };
    void setMethod(const std::string&);
public:
    HttpRequest() = default;
    ~HttpRequest() = default;

    bool analyseFile(const std::string&,HttpContent&);
    void request(TcpClient*,std::string&);

    // 把请求文件的路径加上
    void addFilePath(const std::string&,HttpContent&);
    bool fileIsExist(HttpContent&);
    // 获取文件的类型
    bool analyseFileType(const std::string&,HttpContent&);
    void Header(bool flag,HttpContent& content);

    // 把一些头文件的信息都加进来，只有成功的时候调用这个函数,
    // 并返回文件中的数据
    void processHead(HttpContent& content);
    void addHeader(const std::string& head,HttpContent& content);

    void setResponseFile(std::string&,HttpContent& );
    
private:
    const std::string path_ = "./www/dxgzg_src";
    HttpResponse httpResponse_;
    static std::map<std::string,std::string> contentTypes;
    METHOD method_;
    // 在[]的^是以什么什么开头，放在[]里面的是非的意思
    std::string pattern_ = "^([A-Z]+) ([A-Za-z./1-9-?=]*)";
};


