using System;
using System.Net.Sockets;  //套接字的命名空间
using System.Net;         //IPAddress的命名空间
using System.Collections;
using System.Threading;
using LOGGER = TcpServer.Logger;
using System.Collections.Generic;
using TcpClient = TcpServer.TcpClient;
namespace TcpServer
{
    // 专门负责端口号和IP地址的
    class main
    {

        // 用户自定义的回调函数要正确的处理异常和自己负责关闭套接字
        static void ReadCallback(TcpClient t)
        {
            Socket fd = t.getFd();
            try
            {
                byte[] buff = new byte[1024];
                int cur = fd.Receive(buff);
                if (cur == 0)
                {
                    t.close();
                    return;
                }
                string str = System.Text.Encoding.UTF8.GetString(buff);
                LOGGER.print("客户端发来一条消息: " + str);
                //string msg = "hello client";
                byte[] byteMsg = System.Text.Encoding.Default.GetBytes(str);
                fd.Send(byteMsg);
                // 这个close是用来做ping-pang测试的，就是服务器收到一条消息，
                // 原模原样发送回去，然后关闭这个套接字
                // close();
            }
            catch (System.Net.Sockets.SocketException)
            {
                t.close();
            }
        }
        delegate void Callback();
        //Program p = new Program();
        //    Callback  c = new Callback(p.show);
        //    c();
        enum Event
        {
            nReadEvent = 0,
            nCloseEvent = 0,
        }
        static void Main(string[] args)
        {
         
            EventLoop loop = new EventLoop();
            Acceptor acceptor = new Acceptor(loop);
            acceptor.setThreadNum(2);
            acceptor.setUserDefineReadCallback(ReadCallback);
           // i.setNewConnectionCallback(p.NewConnectionCallback);
            loop.loop();

        }
    }
}
