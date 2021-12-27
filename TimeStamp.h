#pragma once

#include <time.h>
#include <string>


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
		time_t  now =time(0);
		char  * data = ctime(&now);
		tm  * gm =gmtime(&now);
		data = asctime(gm);
		std::string nowString = data;
		nowString.pop_back(); // 去掉最后一个\n
		nowString += " GMT";
		return nowString;
	}
private:
};