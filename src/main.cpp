#include <iostream>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>



#include "Pine_Base.h"
#include "Timer.h"
using namespace std;

void read_cb(int fd,void*){
    char buf[1024] ={ 0 };
    int r = recv(fd,buf,sizeof(buf),0);
    cout<<"read_cb:"<<buf<<endl;
    char* buff = "hello";
    send(fd,buff,strlen(buff),0);
}
void write_cb(int,void*){
    cout<<"write_cb"<<endl;
}
int main(){
    Pine_Base p(read_cb,write_cb);
    string ip = "127.0.0.1";
    p.eventLoop(AF_INET,ip.c_str(),9996);
    return 0;
}