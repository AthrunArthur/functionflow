#!/bin/bash


cd build && cmake ../ && make kmeans_ff kmeans_ff_reuse
cp ../Makefile ./


for i in $(seq 2)
do
  echo $i
  make kmeans_ff
  make kmeans_ff_reuse
done

mv *_time.json ../

cd ../

machine=$(uname -n)
echo "The test is over, please check out your files. The machine is "${machine} | mail -s 'Auto Notification' xuepeng_fan@163.com
