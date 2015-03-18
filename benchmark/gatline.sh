#!/bin/bash

cd build && cmake ../ && make
rm -rf *.json
cp ../makefiles/* ./

CPU_NUM=8

ITER_NUM=2

for i in $(seq ${ITER_NUM})
do
  echo $i sequential iteration
  export THRD_NUM=0
  make -f Makefile.tbb
done
mv tbb_time.json ../sequential_time.json


for i in $(seq ${ITER_NUM})
do
  echo $i performance iteration
  export THRD_NUM=${CPU_NUM}
  make -f Makefile.ff 
  make -f Makefile.tbb
done
mv ff_time.json ../performance_ff_time.json
mv tbb_time.json ../performance_tbb_time.json

for i in $(seq ${ITER_NUM})
do
  echo $i scalibility iteration
  for j in $(seq 2) #$(seq ${CPU_NUM})
  do
    export THRD_NUM=$j
    echo thrd_num is ${THRD_NUM}
    make -f Makefile.ff 
  done
done
mv ff_time.json ../scalibility_ff_time.json

######overhead
for i in $(seq ${ITER_NUM})
do
  echo $i overhead iteration
  export THRD_NUM=${CPU_NUM}
  export TSUIT=base
  make -f Makefile.base ptd_base
  export TSUIT=cmp
  make -f Makefile.base ptd_cmp
done
mv base_time.json ../overhead_base.json
mv cmp_time.json ../overhead_cmp.json


#######static optimizer

for i in $(seq ${ITER_NUM})
do
  echo $i optimizer iteration
  export THRD_NUM=${CPU_NUM}
  export TSUIT=ff_normal
  make -f Makefile.base canny_ff_normal scluster_ff_normal
  export TSUIT=ff_reuse
  make -f Makefile.base canny_ff_reuse scluster_ff_reuse
done
mv ff_normal_time.json ../ff_normal_time.json
mv ff_reuse_time.json ../ff_resue_time.json


cd ../

machine=$(uname -n)
#echo "The test is over, please check out your files. The machine is "${machine} | mail -s 'Auto Notification' xuepeng_fan@163.com

