#!/bin/bash
rm data.json

python ../build.py build
for i in $(seq 30)
do
  echo $i
  cd ../ && python build.py run res.json && cd command/ && mv ../res.json ./
  python bomb.py res.json data.json  original
  rm ../res.json 
done


#./pp.sh
#rm -rf *.pyc
