using System;
using System.Collections.Generic;
using System.Text;

namespace TcpServer
{
    class TimeStamp
    {
        public static string Now()
        {
            return DateTime.Now.ToString();
        }
    }
}
