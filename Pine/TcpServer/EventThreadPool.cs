using System;
using System.Collections.Generic;
using System.Text;
using LOGGER = TcpServer.Logger;
using System.Threading;


namespace TcpServer
{
    class EventThreadPool
    {
        public EventThreadPool()
        {
            threadPool_ = new List<EventThread>();
            nextIndex_ = 0;
        }
        public void setThreadNum(int num)
        {
            for(int i = 0;i < num; ++i)
            {
                EventLoop loop = new EventLoop();
                threadPool_.Add(new EventThread(loop));
            }
        }
        public EventThread getNext()
        {
            if (nextIndex_ == threadPool_.Count)
            {
                nextIndex_ = 0;
            }
            return threadPool_[nextIndex_++];
        }

        private List<EventThread> threadPool_;
        private int nextIndex_; // 下一个线程loop
    }
}
