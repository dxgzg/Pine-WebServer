using System;
using System.Collections.Generic;
using System.Text;
using System.Net;
using System.Net.Sockets;

using EventLoop = TcpServer.EventLoop;
using LOGGER = TcpServer.Logger;
using Channe = TcpServer.Channel;
namespace TcpServer
{
    
    class InetAddr
    {



        private IPAddress ip_;
        private IPEndPoint port_;
        
        public InetAddr(Socket serverSock,string ip = "127.0.0.1",int port = 9996)
        {
           // ip = "192.168.8.112";
            ip_ = IPAddress.Parse(ip);
            port_ = new IPEndPoint(ip_, port);
            serverSock.Bind(port_);// bind一下
            serverSock.Listen(2048);
            LOGGER.print(getPort()+"服务器启动成功");
        }

        string getIp()
        {
            return ip_.ToString();
        }
        string getPort()
        {
            return port_.ToString() + " ";//已经是IP加端口了
        }
        string getIpPort()
        {
            string ipPort = getIp() + ":" + getPort() + " ";
            return ipPort;
        }
    }
}
