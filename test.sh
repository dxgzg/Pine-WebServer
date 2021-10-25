#!/bin/bash  
if [ -f log.txt ];then
    rm -f log.txt
fi;
for((i=0;i<=10;++i))
do
    ./Pine --flagfile server.conf >> log.txt &
    curl http://127.0.0.1:9996/
    workId=`ps -ef | grep Pine | grep -v grep | awk '{print $2}'`
    kill -9 ${workId} 
    echo ${workId}
done