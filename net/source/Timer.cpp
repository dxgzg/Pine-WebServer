#include "Timer.h"
using namespace std;
Timer::Timer(std::uint64_t   when,std::uint64_t expire,
        timerCallback timerCallback,bool repeat)
        :when_(when),expire_(expire),timerCallback_(timerCallback),repeat_(repeat){

}