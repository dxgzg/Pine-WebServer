using System;
using System.Collections.Generic;
using System.Text;
using System.Net;
using System.Net.Sockets;
using System.Threading;


using LOGGER = TcpServer.Logger;
using EventLoop = TcpServer.EventLoop;
using UserDefineReadCallback = TcpServer.Acceptor.UserDefineReadCallback;


namespace TcpServer
{
    class TcpClient
    {
       // public delegate void ReadCallback();

        private EventLoop loop_;
        private Channel channel_;
        private Socket fd_;
        private bool isDefineCallback = false;// 默认用户没有定义
        UserDefineReadCallback userDefineReadCallback_;

        public TcpClient(EventLoop loop,Socket fd)
        {
            loop_ = loop;
            fd_ = fd;
            channel_ = new Channel(loop_,fd_);
         
        }
        public void updateChannel()
        {   
            // 注册回调函数和把client加到套接字中
            update();
            loop_.updateChannel(fd_,channel_);
            
        }
        public Channel getChannel()
        {
            return channel_;
        }
        private void update()
        {
            channel_.setReadCallback(handleConnection);
        }

        public void setUserDefineReadCallback(UserDefineReadCallback userDefineReadCallback)
        {
            isDefineCallback = true;
            userDefineReadCallback_ = new UserDefineReadCallback(userDefineReadCallback);
        }

        // 假若用户自定义了读的回调函数，这个也可以帮助用户处理异常
        public void close()
        {
            LOGGER.print("用户关闭");
            fd_.Shutdown(SocketShutdown.Both);
            fd_.Close();
            loop_.removeChannel(fd_, channel_);
        }
        public EventLoop getLoop()
        {
            return loop_;
        }
        public Socket getFd()
        {
            return fd_;
        }
        private void handleConnection()
        {
            // 如果用户自定义了一个读的回调函数，网络库返回给调用者
            if(isDefineCallback == true)
            {
                userDefineReadCallback_(this);
                return; 
            }

            if (fd_.Poll(0, SelectMode.SelectRead))
            {
                try
                {
                    byte[] buff = new byte[1024];
                    int cur = fd_.Receive(buff);
                    if (cur == 0)
                    {
                        close();
                        return;
                    }
                    string str = System.Text.Encoding.Default.GetString(buff);
                    LOGGER.print("客户端发来一条消息: " + str);
                    //string msg = "hello client";
                    byte[] byteMsg = System.Text.Encoding.Default.GetBytes(str);
                    fd_.Send(buff);
                    // close();

                }
                catch (System.Net.Sockets.SocketException)
                {
                    close();
                }
                    
            }
        }
    }
}
