#!/bin/bash


cd build && cmake ../ && make
cp ../Makefile ./


for i in $(seq 2)
do
  echo $i
  make ff
  make tbb
  make openmp
done

mv *_time.json ../

cd ../

machine=$(uname -n)
echo "The test is over, please check out your files. The machine is "${machine} | mail -s 'Auto Notification' xuepeng_fan@163.com
