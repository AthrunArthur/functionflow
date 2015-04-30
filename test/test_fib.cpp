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
#define BOOST_TEST_MODULE test_ff
#include "ff.h"
#include <iostream>
#include <boost/test/included/unit_test.hpp>
#include <vector>

int64_t fib(int n)
{
	if(n <=2)
		return 1;
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

BOOST_AUTO_TEST_SUITE(fib_test)
BOOST_AUTO_TEST_CASE(fib_t1)
{
  std::vector<int> nums;
  nums.push_back(0);
  nums.push_back(1);
  nums.push_back(2);
  nums.push_back(10);
  for(int i = 0; i < nums.size(); i++)
  {
    int64_t f_res = fib(nums[i]);
    int64_t p_res = sfib(nums[i]);
    BOOST_CHECK(f_res == p_res);
  }
}

BOOST_AUTO_TEST_SUITE_END()
