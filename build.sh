#!/bin/bash
path=`pwd`

buildDir=${path}"/build"
if [ ! -d "$buildDir" ]; then 
mkdir "$buildDir" 
fi 
cd $path/build
rm -rf *
cmake ../
make

