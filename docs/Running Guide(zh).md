1.编译ff：（在主目录下）
	chmod a+x build.sh
	./build.sh 

2.编译benchmark:(在benchmark目录下)
	a.编译并运行所有benchmark程序：
	
		python runBM.py
		
	b.编译并运行选定程序：(可选项有canny、quicksort、lu等)
	
		python runBM.py canny
		
	或
	
		python runBM.py canny quicksort
		
	等。
