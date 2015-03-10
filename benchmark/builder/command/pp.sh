#!/bin/bash

python plot.py data.json originalmutex fixqueuemutex
pdflatex main.tex
mv main.pdf mutex.pdf
./clean.sh


python plot.py data.json originalqsort fixqueueqsort
pdflatex main.tex
mv main.pdf qsort.pdf
./clean.sh

python plot.py data.json originallu fixqueuelu
pdflatex main.tex
mv main.pdf lu.pdf
./clean.sh

python plot.py data.json originalkmeans fixqueuekmeans
pdflatex main.tex
mv main.pdf kmeans.pdf
./clean.sh

python plot.py data.json originalfib fixqueuefib
pdflatex main.tex
mv main.pdf fib.pdf
./clean.sh

python plot.py data.json originalnqueen fixqueuenqueen
pdflatex main.tex
mv main.pdf nqueen.pdf
./clean.sh

open mutex.pdf
open qsort.pdf
open lu.pdf
open kmeans.pdf
open nqueen.pdf
open fib.pdf
