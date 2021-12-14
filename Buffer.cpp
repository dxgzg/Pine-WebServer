#include "Buffer.h"
#include "Logger.h"
#include <string.h>
#include <sys/uio.h>
#include <errno.h>
#include <unistd.h>

using namespace std;

static int BUFFER_LEVEL = 1024 * 1024;

void Buffer::addCapacity(int len){
    // true 就可以开启挤牙膏模式
    if(readIndex_ + writAble() - head > len){
        int readAbleLength = readAble();
        std::copy(readStartIndex(),readStartIndex() + readAbleLength,startIndex());
        readIndex_ = head;
        writeIndex_ = readAbleLength;    
    }
    else{
        
        if(vchar_.size() < BUFFER_LEVEL){
            vchar_.resize(vchar_.size() + len * 2);
        } else{
           vchar_.resize(vchar_.size() + len + BUFFER_LEVEL);// 开启扩容 
        }
    }
}

int Buffer::recvMsg(int fd){
    int N = 65535;
    char extrBuff[65535];
    
    // char* extrBuff = new char[N];
    struct iovec vec[2];
    int writable = writAble();
    vec[0].iov_base = writeStartIndex();
    vec[0].iov_len = writable;

    vec[1].iov_base = extrBuff;
    vec[1].iov_len = N;

    const int iovcnt = (writable < static_cast<int>(sizeof extrBuff)) ? 2 : 1;
    ssize_t n = ::readv(fd,vec,iovcnt);


    LOG_INFO("io read bytes:%d",n);

    if(n == -1)return -1;
    if(writable >= n){
        writeIndex_ += n;
    }
    else if(writable < n){ // 防止 n == -1
        writeIndex_ = vchar_.size();// 已经把他读满了,所以挪到后面去了
        extrBuff[n - writable] = '\0';
        addMessage(extrBuff,::strlen(extrBuff));
    }
    return n;
 }


int Buffer::send(int fd,string& msg){
    // true表示发送成功，false表示需要注册事件。
    size_t n = ::send(fd,msg.c_str(),msg.size(),0);
    return n;
}
