#!/bin/bash
path=`pwd`
cd $path/build
cmake ../
make
cd benchmark
ptime=0
for file in *
do
  if [ -f "$file" -a -x "$file" ]; then
    echo $file":"
    ptime=0
    for i in $(seq 3)
    do
      ./$file
      ptime=$[$ptime+$?]
    done
    ptime=$[$ptime/3];
    echo "Average "$ptime"us"
    echo "End of "$file
  fi
done
