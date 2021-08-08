#include <vector>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>

class Buffer
{
private:
    constexpr static int head = 8;
    constexpr static int InitSize = 1024;
private:
    int readIndex_;
    int writeIndex_;
    std::vector<char> vchar_;
public:
    explicit Buffer(int initsize = InitSize):readIndex_(head),writeIndex_(head),vchar_(head + initsize){}
    ~Buffer() = default;
    char* begin(){
        return vchar_.data();
    }
    char* startIndex(){
        return begin() + head;
    }
    char* startReadIndex(){
        return begin() + readIndex_;
    }
    char* readStartIndex(){
        return begin() + head + readIndex_;
    }
    char* writeStartIndex(){
        return begin() + writeIndex_;
    }
    int readAble()const{
        return writeIndex_ - readIndex_;
    }
    int writAble()const{
        return vchar_.size() - writeIndex_;
    }
    void addCapacity(int len);

    bool isNeedAddCapacity(int len){
        if(len > writAble()){ 
            addCapacity(len); // 自动扩容了
            return true;
        }
        return false;
    }

    void addMessage(char* msg,int len){
        isNeedAddCapacity(len);
        std::copy(msg,msg+len,writeStartIndex());
        writeIndex_ += len;
    }
    
    void retrieve(int len){
        if(len >= readAble()){// 说明都读完了
            readIndex_ = head;
            writeIndex_ = head;
        }
        else{
            readIndex_ += len;
        }
    }

    std::string getAllString(){
        std::string s(startReadIndex(),readAble());// 后面表示拼接字符串的长度
        retrieve(s.size());
        return s;// char*自动转字符串了
    }
    int send(int fd,std::string& msg);

    int recvMsg(int fd);
};