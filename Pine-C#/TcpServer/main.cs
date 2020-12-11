using System;
using System.Net.Sockets;  //套接字的命名空间
using System.Net;         //IPAddress的命名空间
using System.Collections;
using System.Threading;
using LOGGER = TcpServer.Logger;
using System.Collections.Generic;
using TcpClient = TcpServer.TcpClient;
using System.IO;
using System.Text;
using System.Text.RegularExpressions;
using Http = TcpServer.Http;
namespace TcpServer
{
   
    // 专门负责端口号和IP地址的
    class main
    {
        static Http http = new Http();
        static void Main(string[] args)
        {
            Http http = new Http();
            //开启服务器
            EventLoop loop = new EventLoop();
            Acceptor acceptor = new Acceptor(loop);
            acceptor.setThreadNum(8);
            acceptor.setUserDefineReadCallback(http.ReadCallback);
            loop.loop();

        }
    }
}
// 加头文件


//finalHeader(str,ref fileInfo);


//string msg = "hello client";
//byte[] byteMsg = System.Text.Encoding.Default.GetBytes(str);
//string header = "HTTP/1.1 200 OK\r\n";
//if (value.Equals("/"))
//{ // 如果是/的话就给默认值
//    path_ += "\\";
//    path_ += "index.html";
//}
//else
//{ // 暂时发送404
//    string tmpHeader = "HTTP/1.1 404 NOTFOUND\r\nContent-Length:0\r\n\r\n";
//    byte[] byteTmpHeader = System.Text.Encoding.Default.GetBytes(tmpHeader);
//    fd.Send(byteTmpHeader);
//    return;
//}
//header += "Content-Type:text/html\r\n";

//byte[] fileMsg = File.ReadAllBytes(path_);
//int len = fileMsg.Length;
//Console.WriteLine("len" + len);
//header += "Content-Length:" + len.ToString() + "\r\n\r\n";
//byte[] byteHeader = System.Text.Encoding.Default.GetBytes(header);
//// 最开始的测试
////string msg2 = "hello";
////byte[] byteMsg2 = System.Text.Encoding.Default.GetBytes(msg2);
//fd.Send(byteHeader);
//// 发完了头，在发网页信息
//fd.Send(fileMsg);
//Console.WriteLine("发送成功!");
// 这个close是用来做ping-pang测试的，就是服务器收到一条消息，
// 原模原样发送回去，然后关闭这个套接字
// close();
// 文件测试
// main a = new main();
// a.testFile();
// Console.WriteLine(a.path_);
//// FileStream fileStream = new FileStream(a.path_,FileMode.Open);
// byte[] buttf = File.ReadAllBytes(a.path_);
// string str = Encoding.Default.GetString(buttf, 0, buttf.Length);
// Console.WriteLine(str);


////正则表达式测试
/////string path = "F:\\C#BigWork\\www";
//// string str = " GET /style.css HTTP/1.1";
//string str = "GET /js/jquery-3.3.1.min.js HTTP/1.1";
//string pattern = "[/][a-z./1-9-]*";
//Regex reg = new Regex(pattern);
////例如我想提取记录中的NAME值
//Match match = reg.Match(str);
//// 请求的具体文件
//string value = match.Groups[0].Value;
//// 先获取请求的文件
//Console.WriteLine("value:" + value + " length:" + value.Length);
//if (value.Equals("/"))
//{
//    Console.WriteLine("equals / ");
//}
//else
//{
//    Console.WriteLine("not equals / ");
//}
//string tmp = "";
//for(int i = 0;i < value.Length; ++i)
//{
//    if (value[i].Equals('/'))
//    {
//        tmp += "\\";
//    }
//    else
//    {
//        tmp += value[i];
//    }
//}
//path += tmp;
//Console.WriteLine("解析的文件路径为{0}", path);
//string type = "";
//for (int i = 0; i < value.Length; ++i)
//{
//    if (value[i].Equals('.'))
//    {
//        // 获取请求文件以什么结尾的
//        type = value.Substring(i + 1);
//    }
//}
//Console.WriteLine("type:" + type);
//if (type.Equals("css"))
//{
//    // 判断文件的类型
//    Console.WriteLine("type css equals");
//}
//else
//{
//    Console.WriteLine("type css not equals");
//}
//if (type.Equals("js"))
//{
//    // 判断文件的类型
//    Console.WriteLine("type js equals");
//}
//else
//{
//    Console.WriteLine("type js not equals");
//}