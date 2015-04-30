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

//#include <boost/test/unit_test.hpp>
#include "ff.h"
#include <iostream>

using namespace ff;

BOOST_AUTO_TEST_SUITE(minimal_test)
BOOST_AUTO_TEST_CASE(paracontainer_test_all_null)
{
  ff::paracontainer pc;
  ff::ff_wait(all(pc));
}

BOOST_AUTO_TEST_CASE(paracontainer_test_any_null)
{
  ff::paracontainer pc;
  ff::ff_wait(any(pc));
}
BOOST_AUTO_TEST_CASE(paracontainer_test_empty)
{
  ff::paracontainer pc;
  ff::paracontainer pc2;
  ff::para<> a;
  pc.add(a);
  pc2.add(a);
  BOOST_REQUIRE_THROW(ff::ff_wait(all(pc)), empty_para_exception);
  BOOST_REQUIRE_THROW(ff::ff_wait(any(pc2)), empty_para_exception);
}

BOOST_AUTO_TEST_CASE(paracontainer_test_sum)
{
  ff::paracontainer pc;

  int require_sum = 0;
  const static int max = 1000;
  std::atomic_int para_sum(0);
  for(int i = 0; i < max; ++i)
  {
    require_sum += i;
    ff::para<> a;
    a([i, &para_sum](){para_sum += i;});
    pc.add(a);
  }
  ff::ff_wait(all(pc));
  BOOST_CHECK(para_sum.load() == require_sum);
}
BOOST_AUTO_TEST_SUITE_END()
