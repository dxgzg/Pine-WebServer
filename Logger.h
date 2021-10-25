#pragma once
#include <string>
#include <thread>
#include <sstream>
enum class Level{
        INFO,
        ERROR,
        FATAL,
};
#define LOG_INFO(logmsgFormat,...) \
    do \
    { \
        Logger& logger = Logger::instance(); \
        logger.setLevel((int)Level::INFO); \
        char buf[1024] = { 0 }; \
        std::ostringstream oss;\
        oss << std::this_thread::get_id();\
        int n = snprintf(buf,1024,"%s [<%s>%s:%d] ",oss.str().c_str(),__FILE__,__func__,__LINE__);\
        snprintf(buf + n,1024 - n,logmsgFormat,##__VA_ARGS__); \
        logger.print(buf); \
    }while(0)


#define LOG_ERROR(logmsgFormat,...) \
    do \
    { \
        Logger& logger = Logger::instance(); \
        logger.setLevel((int)Level::ERROR); \
        char buf[1024] = { 0 }; \
        snprintf(buf,sizeof(buf),logmsgFormat,##__VA_ARGS__); \
        logger.print(buf); \
    }while(0)


#define LOG_FATAL(logmsgFormat,...) \
    do \
    { \
        Logger& logger = Logger::instance(); \
        logger.setLevel((int)Level::FATAL); \
        char buf[1024] = { 0 }; \
        snprintf(buf,sizeof(buf),logmsgFormat,##__VA_ARGS__); \
        logger.print(buf); \
        exit(-1);\
    }while(0)



class Logger{
public:
    static Logger& instance();
    void print(std::string msg);
    void setLevel(int level);
private:
    
    int logLevel_;
};