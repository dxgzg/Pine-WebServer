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

#include "rapidjson/filewritestream.h"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/filereadstream.h"

#include "EventLoop.h"
#include "TcpServer.h"
#include "TcpClient.h"
#include "HttpServer.h"
#include "Buffer.h"
#include "Logger.h"
#include "TimeStamp.h"
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
void postCb(string type,string args){
    if(type == "/addMsg"){ //  添加留言
        Document tmp;
        tmp.Parse(args.c_str());
        if(tmp.HasParseError()){
            LOG_ERROR("parse args error args:%s",args.c_str());
            return ;
        }
        if(!tmp.HasMember("content")){
            LOG_ERROR("不含有content");
            return ;
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
            return ;
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
    }

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
