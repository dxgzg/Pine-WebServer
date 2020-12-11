using System;
using System.Collections.Generic;
using System.Text;
using System.Net;
using System.Net.Sockets;


using EventThreadPool = TcpServer.EventThreadPool;
using EventLoop = TcpServer.EventLoop;
using LOGGER = TcpServer.Logger;
using Channe = TcpServer.Channel;

using TcpClient = TcpServer.TcpClient;

namespace TcpServer
{
    // 也要将Acceptor的channel注册到EventLoop上面
    class Acceptor
    {  
       
        private NewConnectionCallback newConnectionCallback_;
        private EventLoop loop_;
        private Channel acceptChannel_;
        private Socket serverSock_;
        private InetAddr inetAddr_;
        private EventThreadPool eventThreadPool_;
        private UserDefineReadCallback userDefineReadCallback_;

        //设置回调函数
        public delegate void NewConnectionCallback(Socket fd);
        // 用户(使用该网络库的人)调用这个函数
        public delegate void UserDefineReadCallback(TcpClient t);

        public Acceptor(EventLoop loop)
        {   
            // BaseLoop
            loop_ = loop;
            serverSock_ = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp); ;
            acceptChannel_ = new Channel(loop_,serverSock_);
            inetAddr_ = new InetAddr(serverSock_);
            eventThreadPool_ = new EventThreadPool();
            userDefineReadCallback_ = null;
            update();
        }

        // 可以让使用该网络库的人自定义read回调函数
        public void setUserDefineReadCallback(UserDefineReadCallback userDefineReadCallback)
        {
            userDefineReadCallback_ = new UserDefineReadCallback(userDefineReadCallback);
        }

        // 默认开启的是两个线程
        public void setThreadNum(int num)
        {
            eventThreadPool_.setThreadNum(num);
        }

        // 让用户不设置就使用默认handleNewConnection这个函数，
        // 也可以让用户自定义，自己定义如何去连接socket
        public void setNewConnectionCallback(NewConnectionCallback cb)
        {
            newConnectionCallback_ = new NewConnectionCallback(cb);
        }

        // 设置回调函数
        private void update()
        {
            acceptChannel_.setReadCallback(handleRead);
            loop_.updateChannel(serverSock_, acceptChannel_);
            this.setNewConnectionCallback(handleNewConnection);
        }

        // 通过回调函数调用这个来处理新的连接
        private void handleNewConnection(Socket fd)
        {
            // 怎么能把新连接的客户端放到子线程中去呢？通过上面的轮询算法
            var tmpLoop = eventThreadPool_.getNext().getLoop();
            TcpClient t = new TcpClient(tmpLoop, fd);
            if(userDefineReadCallback_ != null)
            {
                t.setUserDefineReadCallback(userDefineReadCallback_);
            }
            
            t.updateChannel(); // 在这里面注册大量回调
        }
        private void handleRead()
        {
            if (serverSock_.Poll(0, SelectMode.SelectRead))
            {
                Socket connfd = serverSock_.Accept();
                LOGGER.print("服务器收到新的连接 " + connfd.GetHashCode());
                newConnectionCallback_(connfd);// 调用回调函数
            }

        }
    }
}
