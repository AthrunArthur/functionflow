cd build && cmake ../ && make
rm -rf *.json
cp ../makefiles/* ./

CPU_NUM=8

ITER_NUM=3

for i in $(seq ${ITER_NUM})
do
  echo $i scalibility iteration
  for j in $(seq ${CPU_NUM})
  do
    export THRD_NUM=$j
    echo thrd_num is ${THRD_NUM}
    make -f Makefile.ff 
  done
done
mv ff_time.json ../scalibility_ff_time.json

machine=$(uname -n)
echo "The scalibity test is over, please check out your files. The machine is "${machine} | mail -s 'Auto Notification' 1009971856@qq.com

echo "The scalibity test is over, please check out your files. The machine is "${machine} | mail -s 'Auto Notification' xuepeng_fan@163.com
