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

#include <boost/test/included/unit_test.hpp>

#include "ff.h"
using namespace ff;

BOOST_AUTO_TEST_SUITE(minimal_test)
BOOST_AUTO_TEST_CASE(thread_local_var_basic){
  int64_t s = 0;
  ff::thread_local_var<int64_t> ts;
  ff::paracontainer pc;
  for(int i = 0; i < 0xffff; i++)
  {
    s+= i;
    para<> p;
    p([&ts, i](){ts.current() += i;});
    pc.add(p);
  }
  ff_wait(all(pc));
  int rs = 0;
  ts.for_each([&rs](int64_t i){rs += i;});
  BOOST_CHECK(rs == s);
}

BOOST_AUTO_TEST_SUITE_END()
