#include <iostream>
#include "ff.h"


int foo(int t)
{
	std::cout<<t<<std::endl;
	return t + 1;
}
using namespace ff;
int main(int argc, char *argv[])
{
	ff::para a;
	int num = 10;
	a([&num](){foo(num);});
	ff::para b;
	b<:a:>([&num](){foo(11);});
	
	ff::para c;
	//c<: a && b:>([](){foo(12);});
	return 0;
}