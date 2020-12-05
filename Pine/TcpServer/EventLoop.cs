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
                Thread.Sleep(1); // 睡1毫秒，解决一下CPU占用过高的情况。
            }
        }


        public void updateChannel(Socket fd,Channel channel)
        {
            recator_.upChannel(fd,channel);
            addChannel(channel);
        }
        public void removeChannel(Socket fd,Channel channel)
        {
            recator_.removeChannel(fd);
            channels_.Remove(channel);
        }


        private void addChannel(Channel channel)
        {
            channels_.AddLast(channel);
        }

        private Recator recator_ = new Recator();
        private LinkedList<Channel> channels_;
    }
}
