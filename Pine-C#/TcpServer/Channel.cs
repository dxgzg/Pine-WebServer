using System;
using System.Collections.Generic;
using System.Text;
using System.Net;
using System.Net.Sockets;
using LOGGER = TcpServer.Logger;
using EventLoop = TcpServer.EventLoop;
namespace TcpServer
{
    // Channel 专门设置回调函数，C#中的委托函数

    class Channel
    {
        public delegate void EventCallback();

        enum Event
        {
            nReadEvent = 1,
            nCloseEvent = 2,
        }

        public Channel(EventLoop loop,Socket fd)
        {
            loop_ = loop;
            fd_ = fd;
        }
        public void setReadCallback(EventCallback cb)
        {
            readCallback_ = new EventCallback(cb);
            //update();
        }
        
        public void enabling()
        {
            events_ |= (int)Event.nReadEvent;
            //update();
        }
        //private void update()
        //{
        //    loop_.upChannel(this);
        //}
        public void handleEvent()
        {
            readCallback_();
        }
        public void setFd(Socket fd) { fd_ = fd; }
        public Socket getFd()
        {
            return fd_;
        }
        private EventLoop loop_;
        private EventCallback readCallback_;
        private Socket fd_;
        private int events_;
    }
}
