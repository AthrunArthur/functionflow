#!/bin/bash
#cd build && cmake ../&& make
cd build 
for i in $(seq 10000)
do 
echo $i
./fftest
done

