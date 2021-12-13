#include <iostream>
#include <cstring>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <functional>
#include <signal.h>
#include <execinfo.h>
#include <thread>
#include <gflags/gflags.h>
#include <fstream>

#include "rapidjson/filewritestream.h"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/filereadstream.h"
#include "random.h"
#include "Base64.h"

#include "EventLoop.h"
#include "TcpServer.h"
#include "TcpClient.h"
#include "HttpServer.h"
#include "Buffer.h"
#include "Logger.h"
#include "TimeStamp.h"

#if 0
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#endif

using namespace std;
using namespace rapidjson;
constexpr int BACKTRACE_SIZE = 16;
DEFINE_string(func,"echo", "What function to do");

void dump(void)
{
	int j, nptrs;
	void *buffer[BACKTRACE_SIZE];
	char **strings;
	
	nptrs = backtrace(buffer, BACKTRACE_SIZE);
	
	printf("backtrace() returned %d addresses\n", nptrs);
 
	strings = backtrace_symbols(buffer, nptrs);
	if (strings == NULL) {
		perror("backtrace_symbols");
		exit(EXIT_FAILURE);
	}
 
	for (j = 0; j < nptrs; j++)
		printf("  [%02d] %s\n", nptrs - j - 1, strings[j]);
 
	free(strings);
}
 
void signal_handler(int signo)
{
	
#if 0	
	char buff[64] = {0x00};
		
	sprintf(buff,"cat /proc/%d/maps", getpid());
		
	system((const char*) buff);
#endif	
 
	printf("\n=========>>>catch signal %d <<<=========\n", signo);
	
	printf("Dump stack start...\n");
	dump();
	printf("Dump stack end...\n");
 
	signal(signo, SIG_DFL); /* 恢复信号默认处理 */
	raise(signo);           /* 重新发送信号 */
}

void callback(Pine::clientPtr tc,Buffer* inputBuffer){
    string s = inputBuffer->getAllString();
    if(s.empty()){
        return ;
    }
    cout <<__FUNCTION__ << "  收到的消息" << s << endl;
    tc->send(s.c_str());
}
void testEcho(){
    EventLoop loop;
    TcpServer t(&loop);
    t.setThreadNum(1);
    // 需要bind绑定一下
    std::function<void(Pine::clientPtr,Buffer*)> pf = std::bind(callback,std::placeholders::_1,std::placeholders::_2);
    t.setClientReadCallback(pf);
    t.start();
    loop.loop();
}

// 处理业务逻辑代码
bool postCb(string type,string args){
    if(type == "/addMsg"){ //  添加留言
        Document tmp;
        tmp.Parse(args.c_str());
        if(tmp.HasParseError()){
            LOG_ERROR("parse args error args:%s",args.c_str());
            return false;
        }
        if(!tmp.HasMember("content")){
            LOG_ERROR("不含有content");
            return false;
        }
        Document d;
        FILE* fp = fopen("./www/dxgzg_src/msg.json","r");
        char buffer[65536];
        FileReadStream is(fp,buffer,sizeof(buffer));

        d.ParseStream(is);

        Value key(kObjectType);
        Value value;
        const char* str = tmp["content"].GetString();
        value.SetString(str,strlen(str));
        cout << value.GetString() << endl;
        key.AddMember("content",value,d.GetAllocator());
 
        if(!d.HasMember("LeavingMsg")){
            cout << "不含有leavingMsg" << endl;
            return false;
        }

        string now = TimeStamp::Now();
        value.SetString(now.c_str(),now.size());
        key.AddMember("time",value,d.GetAllocator());
        
        d["LeavingMsg"].PushBack(key,d.GetAllocator());
        fclose(fp);

        char writerbuffer[65536];
        fp = fopen("./www/dxgzg_src/msg.json","wr");
        FileWriteStream os(fp,writerbuffer,sizeof(writerbuffer));
    
        Writer<FileWriteStream> writer(os);
        d.Accept(writer);

        fclose(fp);

        return true;
    } else if(type == "/addPicture"){
        Document tmp;
        tmp.Parse(args.c_str());

        if(tmp.HasParseError()){    
            LOG_ERROR("parse args error");
            // LOG_ERROR("parse args error args:%s",args.c_str());
            return true;
        }
        if(!tmp.HasMember("content")){
            LOG_ERROR("不含有content");
            return false;
        }
        if(!tmp.HasMember("passKey")){ // 待改
            LOG_ERROR("不含有passKey");
            return false;
        }
        string passKey = tmp["passKey"].GetString();
        if(passKey != ""){
            cout << tmp["passKey"].GetString() << endl;
            LOG_ERROR("passKey error");
            return false;
        }

        string str = tmp["content"].GetString();
        // cout << str << endl;
        string s(str.data(),30);

        int index = s.find_first_of(",");
        s = s.substr(0,index);
        int start = s.find_first_of("/");
        int end = s.find_first_of(";");
        
        string pictureFmt = s.substr(start + 1,end - start - 1);
        LOG_INFO("picture fmt:%s",pictureFmt.c_str());
        string randomName = getName(16);
        string picname = "www/dxgzg_src/img/";
        picname += randomName;
        picname += ".";
        picname += pictureFmt;
        LOG_INFO("file name:%s",picname.c_str());

        string content(str.data() + index + 1);
        string imgdecode64 = base64_decode(content);
        
        ofstream os(picname,ios::out | ios::binary);
        if(!os || !os.is_open() || os.bad() || os.fail()){
            cout << "file open error" << endl;
            return -1;
        }
        os << imgdecode64;
        os.close();

       
        FILE* fp = fopen("./www/dxgzg_src/pictureList.json","rw");
        char buffer[65536];
        FileReadStream is(fp,buffer,sizeof(buffer));
        string listName = "img/";
        listName += randomName ;
        listName += ".";
        listName += pictureFmt;

        tmp.ParseStream(is);
        Value val(listName.c_str(), tmp.GetAllocator()); 
        tmp["pictureList"].PushBack(val,tmp.GetAllocator());
        fclose(fp);

        char writerbuffer[65536];
        fp = fopen("./www/dxgzg_src/pictureList.json","wr");
        FileWriteStream os2(fp,writerbuffer,sizeof(writerbuffer));
    
        Writer<FileWriteStream> writer(os2);
        tmp.Accept(writer);

        fclose(fp);

        return true;
    }
    return false;
}   
void testHttp(){
    HttpServer http;
    http.setPostReadCallback(std::bind(postCb,std::placeholders::_1,std::placeholders::_2));
    http.run();
}
int main(int argc, char** argv){
    gflags::ParseCommandLineFlags(&argc, &argv, true);
    ::signal(SIGPIPE,SIG_IGN);
    ::signal(SIGSEGV, signal_handler);
    std::ostringstream oss;
    oss << std::this_thread::get_id();
    LOG_INFO("main  thread id:%s start server:%s",oss.str().c_str(),FLAGS_func.c_str());
    if(FLAGS_func == "echo"){
        testEcho();
    } else if(FLAGS_func == "http"){
        testHttp();
    }
    
    return 0;
}
