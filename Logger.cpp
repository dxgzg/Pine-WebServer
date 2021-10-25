#include "Logger.h"
#include "TimeStamp.h"

#include <iostream>
using namespace std;

void Logger::setLevel(int level){
    logLevel_  = level;
}


Logger& Logger::instance(){
    static Logger logger;
    return logger;
}

void Logger::print(std::string msg){
    switch (logLevel_)
    {
    case (int)Level::INFO:{
        cout<<"[INFO]";
        break;
    }
    case (int)Level::ERROR:{
        cout<<"[ERROR]";
        break;
    }
    case (int)Level::FATAL:{
        cout<<"[FATAL]";
        break;
    }
    default:
        break;
    }
    cout<<" "<< TimeStamp::Now() <<" " << msg <<endl;
}