using System;
using System.Net.Sockets;  //套接字的命名空间
using System.Net;         //IPAddress的命名空间
using System.Collections;
using System.Threading;
using LOGGER = TcpServer.Logger;
using System.Collections.Generic;
using TcpClient = TcpServer.TcpClient;
using System.IO;
using System.Text;
using System.Text.RegularExpressions;

namespace TcpServer
{
    class Http
    {
        // 文件的根目录
        private const string path_ = "F:\\C#BigWork\\www";
        private string filePath_;// 具体文件的绝对路径
        private string fileType_;// 请求文件的类型
        private string header_;
        private byte[] fileContent_ = null;

        private void addHeader(string head)
        {
            if (head != null)
            {
                header_ += head;
                header_ += "\r\n";
               // Console.WriteLine("我在这里 head!= null" + header_);
            }
            // 自动加个结尾
            else
            {
                header_ += "\r\n";
               // Console.WriteLine("我在这里 head == null" + header_);
            }
        }
        private void Header(bool flag)
        {
            // 判断要发送的头部 true 表示200 false 表示404
            if(flag == true)
            { 
                header_ = "HTTP/1.1 200 OK\r\n";
            }
            else
            {
               header_ = "HTTP/1.1 404 NOTFOUND\r\nContent-Length:0\r\n\r\n";
            }
        }
        // 把一些头文件的信息都加进来，只有成功的时候调用这个函数,
        // 并返回文件中的数据
        private void processHead()
        {
            string ContentType = "Content-Type:";
            if (fileType_.Equals("html") || fileType_.Equals("js"))
            {
                ContentType += "text/html";
            }
            else if(fileType_.Equals("css"))
            {
                ContentType += "text/css";
            }
            else if(fileType_.Equals("jpg") || fileType_.Equals("png"))
            {
                ContentType += "image/" + fileType_;
            }
            addHeader(ContentType);
            fileContent_ = File.ReadAllBytes(filePath_);
            Console.WriteLine("filePaht:" + filePath_);
            int len = fileContent_.Length;
            string ContentLength = "Content-Length:" + len.ToString();
            addHeader(ContentLength);
            // 最后加了一个结尾
            addHeader(null);
           // Console.WriteLine("process fileContent_:" + );
        }

        // 把请求文件的路径加上
        private void addFilePath(string requestFile)
        {
            string tmp = "";
            for (int i = 0; i < requestFile.Length; ++i)
            {
                if (requestFile[i].Equals('/'))
                {
                    tmp += "\\";
                }
                else
                {
                    tmp += requestFile[i];
                }
            }
            filePath_ += tmp;
        }
        private void analyseFileType(string requestFile)
        {
            for (int i = 0; i < requestFile.Length; ++i)
            {
                if (requestFile[i].Equals('.'))
                {
                    // 获取请求文件以什么结尾的
                    fileType_ = requestFile.Substring(i + 1);
                }
            }
        }
        private bool analyseFile(string request)
        {
            // 调用header的
            string pattern = "[/][a-z./1-9-]*";
            Regex reg = new Regex(pattern);
            //例如我想提取记录中的NAME值
            Match match = reg.Match(request);
            // 请求的具体文件
            string requestFile = match.Groups[0].Value;
            // 先获取请求的文件
            Console.WriteLine(requestFile);
            if (requestFile.Equals("/"))
            { // 如果是/的话就给默认值
                filePath_ = ""; // 先清个零
                filePath_ = path_;
                filePath_ += "\\";
                filePath_ += "index.html";

                // 文件的类型也要给人家加上
                fileType_ = "html"; 
            }
            else
            {
                filePath_ = ""; // 先清个零
                filePath_ = path_;
                addFilePath(requestFile);
                // 判断一下文件是否存在,文件不存在的话
                if (File.Exists(filePath_) == false)
                {
                    Logger.print("请求的文件不存在");
                    return false;
                }
            }
            // 如果文件不存在的话也就不需要解析类型
            analyseFileType(requestFile);
            Logger.print("请求的文件存在");
            return true;
        }
        private void SendFile(Socket clientFd, byte[] header, byte[] repose)
        {
            // 头部一定是有的。
            clientFd.Send(header);
            // 发完了头，在发请求文件的信息。如果是404这里是没有的
            if (repose != null)
            {
                //byte[] tmpRepose = File.ReadAllBytes(filePath_);
                clientFd.Send(repose);
            }

        }


        // 用户自定义的回调函数要正确的处理异常和自己负责关闭套接字
        public void ReadCallback(TcpClient t)
        {
            Socket fd = t.getFd();
            try
            {
                byte[] buff = new byte[1024];
                int cur = fd.Receive(buff);
                if (cur == 0)
                {
                    t.close();
                    return;
                }
                string str = System.Text.Encoding.UTF8.GetString(buff);
                LOGGER.print("客户端发来一条请求: " + str);
                // 未找到文件直接回应404.
                bool flag = analyseFile(str);
                Header(flag);
                // 这个也是测试用的
                string tmpHead = header_;
                if (flag == false)
                {
                    byte[] byteHeader = System.Text.Encoding.Default.GetBytes(header_);
                    SendFile(fd, byteHeader, null);
                    return;
                }
                // 这个修改头文件的，先调用这个
                processHead();
                //这个没有复用是因为成功后消息头会改变的
                byte[] byteHead = System.Text.Encoding.Default.GetBytes(header_);
                //这是文件找到了发送的
                // Logger.print("header为:" + header_);
                SendFile(fd, byteHead, fileContent_);
                // 发完就关闭连接
                // t.close();
            }
            catch (System.Net.Sockets.SocketException)
            {
                t.close();
            }
        }
        
    }
}
