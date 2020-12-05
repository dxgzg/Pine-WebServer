using System;
using System.Collections.Generic;
using System.Text;
using System.Threading;
using LOGGER = TcpServer.Logger;
namespace TcpServer
{

    class EventThread
    {
        public EventThread(EventLoop loop)
        {
            thread_ = new Thread(this.start);
            loop_ = loop;
            //channel_ = new Channel(loop_);

            thread_.Start();
        }
        public void start()
        {
            loop_.loop();
        }

        public EventLoop getLoop() { return loop_; }
        private EventLoop loop_;
        private Thread thread_;
        //private Channel channel_;
    }
    
}
