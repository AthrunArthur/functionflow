
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


int main(int argc, char *argv[])
{	
	int num = 10;	
	int fib_res = fib(num);
	std::cout<<"fib( "<<num<<" )="<<fib_res<<std::endl;
	return 0;
}
