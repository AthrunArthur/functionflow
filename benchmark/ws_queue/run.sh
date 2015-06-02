#!/bin/bash

cd build && make
for i in $(seq 10)
do
    echo $i
    echo "*****gcc"
    ./gccm
    echo "*****cpp11"
    ./cppm
    echo ""
done

