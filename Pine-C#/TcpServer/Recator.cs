using System;
using System.Collections.Generic;
using System.Text;
using System.Net;
using System.Net.Sockets;
using Channel = TcpServer.Channel;
using LOGGER = TcpServer.Logger;
namespace TcpServer
{
    /* 在Linux中相当epoll模型，手码了一个类似select的网络模型
    *  我也很无奈，其一我不是在Linux下写的C#，
    *  其二我也不太了解C#有哪些底层的网络模型可以使用
    */

    class Recator
    {
        public Recator()
        {
            channels_ = new Dictionary<Socket, Channel>();
        }
        
        public void upChannel(Socket fd,Channel channel)
        {
            LOGGER.print("ADD");
            if (!channels_.ContainsKey(fd))
            {
                channels_.Add(fd, channel);
            }
        }
        public void removeChannel(Socket fd)
        {
            if(channels_.ContainsKey(fd) == true)
            {// 包含的话就将他删掉
                channels_.Remove(fd);
                LOGGER.print("remove");
            }
        }
        public void poll(ref LinkedList<Channel> activeList)
        {
            fillAcrivec(ref activeList);
        }
        private void fillAcrivec(ref LinkedList<Channel> activeList)
        {
            foreach (var it in channels_)
            {
                Socket fd = it.Key;
                // 在这里监听套接字，如果有活跃的注册添加到活跃里面去
                if (fd.Poll(0, SelectMode.SelectRead))
                {// 谁可以读的话加在activeList中
                    activeList.AddLast(it.Value);
                } 
            }
        }
        //private Socket serverSock_;
        //private LinkedList<Channel> list_;
        private Dictionary<Socket, Channel> channels_;

    }
}
