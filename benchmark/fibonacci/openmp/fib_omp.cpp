#include <omp.h>
#include <cstdint>

#define MINPARA 25
using namespace std;

extern int64_t sfib(int64_t);
int64_t fib(int64_t n)
{
	if(n < MINPARA)
		return sfib(n);
	int64_t x,y;
	#pragma omp task shared(x)
	{
		x = fib(n - 1);
	}
	#pragma omp task shared(y)
	{
		y = fib(n - 2);
	}
	#pragma omp taskwait
	return x+y;
}

