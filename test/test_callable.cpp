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

void func1(){}

void func2(int a){}

auto l1 = [](){return 1;};
auto l2 = std::function<void(int)>(func2);

struct MF1{
  void operator ()(){}
};
struct MF2{
  void operator ()(int a){}
};

struct MF3{
  void ff(){}
};

BOOST_AUTO_TEST_SUITE(minimal_test)

BOOST_AUTO_TEST_CASE(normal_func_test_case)
{
  bool b = ff::utils::is_callable<decltype(func1)>::value;
  BOOST_CHECK(b == true);
  b = ff::utils::is_callable<decltype(func2)>::value;
  BOOST_CHECK(b == true);
  int c;
  b = ff::utils::is_callable<decltype(c)>::value;
  BOOST_CHECK(b == false);
}

BOOST_AUTO_TEST_CASE(cpp11_test_case)
{
  bool b;
  b = ff::utils::is_callable<decltype(l1)>::value;
  BOOST_CHECK(b == true);
  
  b = ff::utils::is_callable<decltype(l2)>::value;
  BOOST_CHECK(b == true);
}

BOOST_AUTO_TEST_CASE(functor_test_case)
{
  bool b;
  b = ff::utils::is_callable<MF1>::value;
  BOOST_CHECK(b == true);

  b = ff::utils::is_callable<MF2>::value;
  BOOST_CHECK(b == true);

  b = ff::utils::is_callable<MF3>::value;
  BOOST_CHECK(b == false);
}

template<class F>
void checker(F && f, bool req)
{
    bool b = ff::utils::is_callable<F>::value;
    BOOST_CHECK(b == req);
}
BOOST_AUTO_TEST_CASE(param_pass1_test_case)
{
  checker(func1, true);
  checker(func2, true);
}

BOOST_AUTO_TEST_CASE(param_pass2_test_case)
{
  checker([](){}, true);
  checker(l1, true);
  checker(l2, true);
}

BOOST_AUTO_TEST_CASE(param_pass3_test_case)
{
  MF1 a;
  checker(a, true);
  MF3 b;
  checker(b, false);
}

BOOST_AUTO_TEST_SUITE_END()
