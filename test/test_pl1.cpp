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
#include <sstream>

BOOST_AUTO_TEST_SUITE(pipeline1_test)
BOOST_AUTO_TEST_CASE(pip1)
{
  ff::input_filter<int> f1;
  f1([]()->int{return 10;});
  ff::filter<std::string> f2;
  f2[f1]([](int d){
      std::stringstream ss;
      ss<< d;
      return ss.str();
      });
  f1.start();
  f2.then([](std::string s){
      BOOST_CHECK(s == "10");
      });
}
BOOST_AUTO_TEST_SUITE_END()