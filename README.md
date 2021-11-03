此网络库仿照的也是muduo网络库，one loop per thread 的模型，自己封装了HTTP协议。这个web server主要是帮助了前端工程师建立一个自己的网站，如果不需要POST请求，单纯是为了让人们看到你的页面使用此web server是一个不错的选择，因为配置真的是极其简单。如果需要POST请求处理一些业务逻辑，如我自己网站的留言版，在main.cpp可以写一个postCallback，把回调函数注册到web server中，当有post请求发生时候可以自行写业务代码。


万事都齐全的情况
```
  # 外部依赖库:gflags库
  chmod +x autobuild.sh # 加一个可执行权限
  ./autobuild.sh # 就可以跑起来了
```

如果cmake make 库也没有，可以下载一个docker，来拉取我的镜像，以centos为例
```
    sudo yum install docker
    docker pull dxgzg/pine:1.0
    docker image ls # 查看一下是否成功拉取镜像
    docker run -p 80:9996 -it dxgzg/pine:1.0 /bin/bash # 80是你通过的外部接口，镜像名字有可能不叫dxgzg/pine:1.0这个名字
    # 接下来就进容器里面来了
    vim server.conf # 更改你的配置项和设置html页面的路径
    # 可以研究一下怎么把外部文件放进容器中，这个有博客介绍过，我没研究过，等有时间这里再补充一下
    ./Pine --flagfile server.conf # 就启动了
```

<a href="http://dxgzg.site">个人网站链接</a>
