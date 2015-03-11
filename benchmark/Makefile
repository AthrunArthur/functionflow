
.PHONY
all: ff tbb openmp


ff: canny_ff fib_ff kmeans_ff lu_ff nqueen_ff qsort_ff

tbb: canny_tbb fib_tbb kmeans_tbb lu_tbb nqueen_tbb qsort_tbb

openmp: canny_openmp fib_openmp kmeans_openmp lu_openmp nqueen_openmp qsort_openmp

#####################
canny_ff:
	cd canny
	./canny_ff --thrd-num 8 && cat time.json >> ../ff_time.json
	cd ../

fib_ff:
	cd fibonacci
	./fib_ff --thrd-num 8 && cat time.json >> ../ff_time.json
	cd ../

kmeans_ff:
	cd kmeans
	./kmeans_ff --thrd-num 8 && cat time.json >> ../ff_time.json
	cd ../

lu_ff:
	cd LU
	./lu_ff --thrd-num 8 --div 256 && cat time.json >> ../ff_time.json
	cd ../

nqueen_ff:
	cd n_queen
	./nqueen_ff --thrd-num 8 && cat time.json >> ../ff_time.json
	cd ../

qsort_ff:
	cd quick_sort 
	./qsort_ff --thrd-num 8 && cat time.json >> ../ff_time.json
	cd ../

#####################
canny_tbb:
	cd canny
	./canny_tbb --thrd-num 8 && cat time.json >> ../ff_time.json
	cd ../

fib_tbb:
	cd fibonacci
	./fib_tbb --thrd-num 8 && cat time.json >> ../ff_time.json
	cd ../

kmeans_tbb:
	cd kmeans
	./kmeans_tbb --thrd-num 8 && cat time.json >> ../ff_time.json
	cd ../

lu_tbb:
	cd LU
	./lu_tbb --thrd-num 8 --div 256 && cat time.json >> ../ff_time.json
	cd ../

nqueen_tbb:
	cd n_queen
	./nqueen_tbb --thrd-num 8 && cat time.json >> ../ff_time.json
	cd ../

qsort_tbb:
	cd quick_sort 
	./qsort_tbb --thrd-num 8 && cat time.json >> ../ff_time.json
	cd ../

