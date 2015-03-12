#!/bin/bash


cd build && cmake ../ && make
cp ../Makefile ./


for i in $(seq 2)
do
  echo $i
  make ff
done

mv *_time.json ../

cd ../

echo "The test is over, please check out your files!" | mail -s 'Auto Notification' xuepeng_fan@163.com
