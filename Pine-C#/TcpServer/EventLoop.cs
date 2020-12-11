using System;
using System.Collections.Generic;
using System.Text;
using System.Net;
using System.Net.Sockets;
using LOGGER = TcpServer.Logger;
using Channel = TcpServer.Channel;
using Recator = TcpServer.Recator;
using System.Threading;


namespace TcpServer
{
    class EventLoop
    {   
        public EventLoop()
        {
            channels_ = new LinkedList<Channel>();
            pending_ = new List<Channel>();
        }
        // 事件开始循环监听
        public void loop()
        {
            while (true)
            {
                LinkedList<Channel> activeLists = new LinkedList<Channel>();
                recator_.poll(ref activeLists);
                foreach(var activeList in activeLists)
                {
                    activeList.handleEvent();
                }
                HandlePending();
                Thread.Sleep(1); // 睡1毫秒，解决一下CPU占用过高的情况。
            }
        }

        public void HandlePending()
        {   
            List<Channel> tmp = new List<Channel>();
            lock (lock_)
            {
                /* 避免了busy loop，mainLoop给subLoop递交新连接。如果直接for，
                *  subLoop处理跟不上就会阻塞mainLoop，
                *  mainLoop阻塞的话就会影响新来的连接
                *  这里本应该用swap来交换，无奈C#不可以这样写，所以就写成如下代码
                *  这是来自muduo网络库的细节！
                */ 
                tmp = pending_;
                pending_ = new List<Channel>();
            }
            for (int i = 0; i < tmp.Count; ++i)
            {
               Channel channel = tmp[i];
               Socket fd = channel.getFd();
               recator_.upChannel(fd, channel);
               addChannel(channel);
            }
         }

        public void updateChannel(Socket fd,Channel channel)
        {
            lock (lock_)
            {
                // 提交给子线程
                channel.setFd(fd);
                pending_.Add(channel);
            }
        }
        public void removeChannel(Socket fd,Channel channel)
        {
            lock (lock_)
            {
                recator_.removeChannel(fd);
                channels_.Remove(channel);
            }
        }


        private void addChannel(Channel channel)
        {
            channels_.AddLast(channel);
        }

        private Recator recator_ = new Recator();
        private LinkedList<Channel> channels_;
        private List<Channel> pending_;
        private readonly object lock_ = new object();
    }
}
