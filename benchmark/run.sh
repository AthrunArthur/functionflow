#!/bin/bash


cd build && cmake ../ && make
cp ../Makefile ./


for i in $(seq 2)
do
  echo $i
  make ff
done


cd ../
