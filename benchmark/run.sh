#!/bin/bash


cd build && cmake ../ && make

for i in $(seq 30)
do
  echo $i
  cd canny
  ./canny_ff && cat time.json >> ../../time.json
  cd ../

  cd ../ && python build.py run res.json && cd command/ && mv ../res.json ./
  python bomb.py res.json data.json  original
  rm ../res.json 
done

cd ../
