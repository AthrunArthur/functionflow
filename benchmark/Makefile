all : ff tbb openmp

UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Darwin)
ff: fib_ff kmeans_ff lu_ff nqueen_ff qsort_ff scluster_ff

tbb: fib_tbb kmeans_tbb lu_tbb nqueen_tbb qsort_tbb

openmp: fib_openmp kmeans_openmp lu_openmp nqueen_openmp qsort_openmp

endif
ifeq ($(UNAME_S),Linux)
ff: canny_ff fib_ff kmeans_ff lu_ff nqueen_ff qsort_ff scluster_ff

tbb: canny_tbb fib_tbb kmeans_tbb lu_tbb nqueen_tbb qsort_tbb

openmp: canny_openmp fib_openmp kmeans_openmp lu_openmp nqueen_openmp qsort_openmp

endif

ff_reuse: kmeans_ff_reuse

kmeans_ff_reuse:
	cd kmeans && ./kmeans_ff_reuse --thrd-num 8 && cat time.json >> ../ffreuse_time.json && cd ../

scluster_ff_reuse:
	cd scluster && ./scluster_ff_reuse --thrd-num 8 && cat time.json >> ../ffreuse_time.json && cd ../



##################### ff
canny_ff:
	cd canny && ./canny_ff --thrd-num 8 && cat time.json >> ../ff_time.json && cd ../

fib_ff:
	cd fibonacci && ./fib_ff --thrd-num 8 && cat time.json >> ../ff_time.json && cd ../

kmeans_ff:
	cd kmeans && ./kmeans_ff --thrd-num 8 && cat time.json >> ../ff_time.json && cd ../

lu_ff:
	cd LU && ./lu_ff --thrd-num 8 && cat time.json >> ../ff_time.json && cd ../

nqueen_ff:
	cd n_queen && ./nqueen_ff --thrd-num 8 && cat time.json >> ../ff_time.json && cd ../

qsort_ff:
	cd quick_sort &&./qsort_makedata && ./qsort_ff --thrd-num 8 --div 256 && cat time.json >> ../ff_time.json && cd ../

scluster_ff:
	cd scluster && ./scluster_ff --thrd-num 8 && cat time.json >> ../ff_time.json && cd ../

#####################tbb
canny_tbb:
	cd canny && ./canny_tbb --thrd-num 8 && cat time.json >> ../tbb_time.json && cd ../

fib_tbb:
	cd fibonacci && ./fib_tbb --thrd-num 8 && cat time.json >> ../tbb_time.json && cd ../

kmeans_tbb:
	cd kmeans && ./kmeans_tbb --thrd-num 8 && cat time.json >> ../tbb_time.json && cd ../

lu_tbb:
	cd LU && ./lu_tbb --thrd-num 8 && cat time.json >> ../tbb_time.json && cd ../

nqueen_tbb:
	cd n_queen && ./nqueen_tbb --thrd-num 8 && cat time.json >> ../tbb_time.json && cd ../

qsort_tbb:
	cd quick_sort &&./qsort_makedata && ./qsort_tbb --thrd-num 8 --div 256 && cat time.json >> ../tbb_time.json && cd ../

###################openmp
canny_openmp:
	cd canny && ./canny_openmp --thrd-num 8 && cat time.json >> ../openmp_time.json && cd ../

fib_openmp:
	cd fibonacci && ./fib_openmp --thrd-num 8 && cat time.json >> ../openmp_time.json && cd ../

kmeans_openmp:
	cd kmeans && ./kmeans_openmp --thrd-num 8 && cat time.json >> ../openmp_time.json && cd ../

lu_openmp:
	cd LU && ./lu_openmp --thrd-num 8 && cat time.json >> ../openmp_time.json && cd ../

nqueen_openmp:
	cd n_queen && ./nqueen_openmp --thrd-num 8 && cat time.json >> ../openmp_time.json && cd ../

qsort_openmp:
	cd quick_sort &&./qsort_makedata && ./qsort_openmp --thrd-num 8 --div 256 && cat time.json >> ../openmp_time.json && cd ../
