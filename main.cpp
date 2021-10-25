#include <iostream>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <functional>
#include <signal.h>
#include <execinfo.h>
#include <thread>
#include <gflags/gflags.h>

#include "EventLoop.h"
#include "TcpServer.h"
#include "TcpClient.h"
#include "HttpServer.h"
#include "Buffer.h"
#include "Logger.h"
using namespace std;
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
void testHttp(){
    HttpServer http;
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
