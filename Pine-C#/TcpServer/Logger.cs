using System;
using System.Threading;

namespace TcpServer
{
    class Logger
    {
        private static readonly object Lock = new object();
        public static void print(string msg)
        {
            /* 为什么要加个锁呢？
            *  因为我偷了个懒在日志中打印出来线程ID，而不加锁会造成两个危害(经检测)
            *  一个是会造成线程ID的紊乱，其次是会把C#封装的线程库搞崩
            *  我的程序在测试的时候发现会崩几次，究其原因就是这里没有加锁
            *  故在此加锁，会略微影响程序的性能，也可以不加锁的，在打印中输出线程的ID
            *  这也是本程序唯一加锁的地方
            */
            lock (Lock)
            {

                Console.WriteLine("[INFO] " + TcpServer.TimeStamp.Now() + " Thread id = " + Thread.GetCurrentProcessorId() + " " + msg);
            }

            {

                //Console.WriteLine("[INFO] "  + " " + msg);
            }
        }
    }
}
