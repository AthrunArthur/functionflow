#!/bin/bash

cd build && cmake ../ && make
rm -rf *.json
cp ../makefiles/* ./

CPU_NUM=8

ITER_NUM=3

for i in $(seq ${ITER_NUM})
do
  echo $i sequential iteration
  export THRD_NUM=0
  make -f Makefile.tbb treesum_tbb
done
mv tbb_time.json ../sequential_time.json


for i in $(seq ${ITER_NUM})
do
  echo $i performance iteration
  export THRD_NUM=${CPU_NUM}
  make -f Makefile.ff treesum_ff
  make -f Makefile.tbb treesum_tbb
  #make -f Makefile.ff 
  #make -f Makefile.tbb
done
mv ff_time.json ../performance_ff_time.json
mv tbb_time.json ../performance_tbb_time.json

machine=$(uname -n)
echo "The test is over, please check out your files. The machine is "${machine} | mail -s 'Auto Notification' 1009971856@qq.com

