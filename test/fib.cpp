
#include "ff.h"
#include "common/log.h"
#include <iostream>

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
int sfib(int n )
{
 if(n <= 2)
   return 1;
 return sfib(n -1 ) + sfib(n-2);
}


int main(int argc, char *argv[])
{
	_DEBUG(ff::log<>::init(ff::INFO, "log.txt"))
	_DEBUG(LOG_INFO(main)<<"main start");
	
	int num = 80;	
	
	//int fib_res = fib(num);
	int fib_res = sfib(num);
	std::cout<<"fib( "<<num<<" )="<<fib_res<<std::endl;
	return 0;
}
