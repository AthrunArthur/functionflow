Function Flow依赖于C++11,因此请首先确定你的编译器支持完整的C++11,已经确认的、适用的编译器有g++ 4.8.1以及clang 3.4。

1.编译ff：（在主目录下）
	
	mkdir build
	cd build
FF的编译有两种版本，debug版本和release版本，默认为debug版本，如下

	cmake ../
注意，debug版本需要更多的依赖，需要连接网络！

release版本的编译如下

	cmake -DRelease=1 ../

2.编译benchmark:(在benchmark目录下，需要FF的release版本)

 a.编译并运行所有benchmark程序：
	
		python runBM.py
		
 b.编译并运行选定程序：(可选项有canny、quicksort、lu等)
	
		python runBM.py canny
		
 或
	
		python runBM.py canny quicksort
		
 等。

3. 编译test：（在test目录下，需要FF的debug版本）

	mkdir build
	cd build
	cmake ../

注意，test需要依赖boost_test！