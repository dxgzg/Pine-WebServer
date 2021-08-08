#pragma once
#include <string>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "TcpClient.h"
#include "Logger.h"


struct HttpContent
{
    std::string filePath_;// 具体文件的绝对路径
    std::string fileType_;// 请求文件的类型
    std::string header_;  // http头response
    size_t fileSize_;
    int fileFd_;
    struct stat fileStat_;
};
