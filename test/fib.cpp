
#include "ff.h"
//#include "common/log.h"


int fib(int n)
{
	if(n <=2)
		return 1;
	//LOG_INFO(main)<<"fib "<<n;
	ff::para<int> a, b;
	a([&n]()->int{return fib(n - 1);});
	b([&n]()->int{return fib(n - 2);});
	return (a && b).then([](int x, int y){return x + y;});
}

int foo(int t)
{
	std::cout<<"foo:"<<t<<std::endl;
	t ++;
	return t;
}

int main(int argc, char *argv[])
{	
	//ff::log<>::init(ff::INFO, "log.txt");
	//LOG_INFO(main)<<"main start";
	int num = 10;	
	
	ff::para<> a;
	a([&num](){return foo(num);});
	
	ff::para<> b;
	b([&num](){foo(num);});
	ff::para<> c;
	c([&num](){foo(num);});
	
	ff_wait(a);
	ff_wait(a && b && c);
	//(a&& b && c).then([](){std::cout<<"a && b"<<std::endl;});
	
	
	
	//ff::para<> b;
	//b[a]([&num, &a](){foo(num + a.get());}).then([](){std::cout<<"do anything here!"<<std::endl;});
	
	
	
	//int fib_res = fib(num);
	//std::cout<<"fib( "<<num<<" )="<<fib_res<<std::endl;
	//LOG_INFO(main)<<"main exit!";
	//ff::rt::join();
	return 0;
}
