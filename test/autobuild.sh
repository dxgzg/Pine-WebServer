#!/bin/bash

# if [ ! -d `pwd`/test ];then
#     mkdir `pwd`/test
# fi
if [ -d `pwd`/build ];then
    rm -rf `pwd`/build
fi
# ÷ÿ–¬±‡“Î
mkdir `pwd`/build

cd build
cmake ..
make
cp libpine.so /usr/lib/

cd ..

if [ ! -d /usr/include/Pine ];then
	mkdir /usr/include/Pine
fi

for header in `ls *.h`
do
	cp $header /usr/include/Pine
done

ldconfig
