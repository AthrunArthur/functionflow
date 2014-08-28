/***********************************************
The MIT License (MIT)

Copyright (c) 2012 Athrun Arthur <athrunarthur@gmail.com>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*************************************************/
#include "ff.h"
#include "common/log.h"
#include <iostream>

int64_t fib(int n)
{
	if(n <=2)
		return 1;
	_DEBUG(LOG_INFO(main)<<"fib "<<n;)
	ff::para<int64_t> a, b;
	a([&n]()->int64_t{return fib(n - 1);});
	b([&n]()->int64_t{return fib(n - 2);});
	return (a && b).then([](int64_t x, int64_t y){return x + y;});
}
int64_t sfib(int n )
{
 if(n <= 2)
   return 1;
 return sfib(n -1 ) + sfib(n-2);
}


int main(int argc, char *argv[])
{
	_DEBUG(ff::fflog<>::init(ff::INFO, "log.txt"))
	_DEBUG(LOG_INFO(main)<<"main start, id:"<<ff::rt::get_thrd_id());
	
	int num = 20;	
	
	int64_t fib_res = fib(num);
	int64_t p_res = sfib(num);
        if (fib_res == p_res)
          std::cout<<"fib correct!!"<<std::endl;
        else
        {
          std::cout<<"Something is wrong here! The result is wrong!"<<std::endl;
        }
	std::cout<<"fib( "<<num<<" )="<<fib_res<<std::endl;
	return 0;
}
