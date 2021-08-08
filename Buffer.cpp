#include "Buffer.h"
#include <string.h>
#include <iostream>
#include <sys/uio.h>
#include <errno.h>
#include <unistd.h>
using namespace std;

void Buffer::addCapacity(int len){
    // true 就可以开启挤牙膏模式
    if(readIndex_ + writAble() - head > len){
        int readAbleLength = readAble();
        std::copy(readStartIndex(),readStartIndex() + readAbleLength,startIndex());
        readIndex_ = head;
        writeIndex_ = readAbleLength;    
    }
    else{
        vchar_.resize(vchar_.size() + len);// 开启扩容
    }
}

int Buffer::recvMsg(int fd){
    char extrBuff[65536];
    struct iovec vec[2];
    ssize_t writable = writAble();
    vec[0].iov_base = writeStartIndex();
    vec[0].iov_len = writable;

    vec[1].iov_base = extrBuff;
    vec[1].iov_len = sizeof(extrBuff);
    const int iovcnt = (writable < sizeof extrBuff) ? 2 : 1;
    ssize_t n = ::readv(fd,vec,iovcnt);
    if(n == -1)return -1;
    if(writable >= n){
        writeIndex_ += n;
    }
    else if(writable < n){ // 防止 n == -1
        writeIndex_ = vchar_.size();
        addMessage(extrBuff,::strlen(extrBuff));
    }
    return n;
 }


int Buffer::send(int fd,string& msg){
    // true表示发送成功，false表示需要注册事件。
    size_t n = ::send(fd,msg.c_str(),msg.size(),0);
    return n;
}
