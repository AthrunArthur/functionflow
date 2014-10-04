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

#include <boost/test/unit_test.hpp>

#include <iostream>
#include <vector>
#include <algorithm>
#include "ff.h"
#include "common/log.h"

using namespace ff;
#define FF_TEST_TIME 100


BOOST_AUTO_TEST_SUITE(minimal_test)


int inc(int t)
{
	return t + 1;
}
double inc(double t)
{
	return t + 1;
}

BOOST_AUTO_TEST_CASE(para_test_simple)
{
    para<> a;
    para<int> b;
    a([](){});
    b([](){return 10;});
    ff_wait(a);
    ff_wait(b);
    BOOST_CHECK(b.get() == 10);
    BOOST_CHECK(a.check_if_over());
    BOOST_CHECK(b.check_if_over());
}

BOOST_AUTO_TEST_CASE(para_test_then)
{
    int num = 10;
    para<int> a;
    a([&num](){return inc(num);}).then([num](int n){BOOST_CHECK(n == inc(num));}); 
    BOOST_CHECK(a.check_if_over());
}

BOOST_AUTO_TEST_CASE(para_test_empty)
{
  int num = 10;
  double b_res;
  para<int> a;
  para<> b;

  BOOST_REQUIRE_THROW(
      b[a]([&num, &a, &b_res](){b_res=inc(num + a.get());}).then([&num, &a, &b, &b_res](){
        BOOST_CHECK(b_res == inc(num + a.get()));
      }), empty_para_exception); 
  BOOST_REQUIRE_THROW(ff_wait(b), empty_para_exception);
  BOOST_REQUIRE_THROW(ff_wait(a), empty_para_exception);
  BOOST_CHECK(!a.check_if_over());
  BOOST_CHECK(!b.check_if_over());
}

BOOST_AUTO_TEST_CASE(para_test)
{
    int num = 10;
    para<int> a;
    a([&num](){return inc(num);}).then([&num](int x){
        BOOST_CHECK(x == inc(num));
    });
    ff::para<> b;
    int b_res;
    b[a]([&num, &a, &b_res](){b_res = inc(num + a.get());}).then([&num, &a, &b, &b_res](){
        BOOST_CHECK(b_res == inc(num + a.get()));
    });
}

BOOST_AUTO_TEST_SUITE_END()
