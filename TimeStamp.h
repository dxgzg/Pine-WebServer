#pragma once

#include <time.h>
#include <string>
#include <chrono>
#include "Logger.h"

class TimeStamp{ 
public:
    static std::string Now(){
        char buff[128] = { 0 };
        time_t microSecondsSinceEpch = ::time(0);
        tm* tm_time = localtime(&microSecondsSinceEpch);
	    snprintf(buff, sizeof(buff), "%4d/%02d/%02d %02d:%02d:%02d",
		tm_time->tm_year + 1900,
		tm_time->tm_mon + 1,
		tm_time->tm_mday,
		tm_time->tm_hour,
		tm_time->tm_min,
		tm_time->tm_sec
		);
        return buff;
    }
	static std::string getGMT(){
        auto now = std::chrono::system_clock::now();

        std::time_t nowTime = std::chrono::system_clock::to_time_t(now);
        tm* tm_time = localtime(&nowTime);
        char buff[128] = { 0 };
        strftime(buff,128,"%a,%d %b %Y %X UTC",tm_time);
        return buff;
	}
private:
};