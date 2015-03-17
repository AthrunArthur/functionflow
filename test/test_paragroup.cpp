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
#include "common/log.h"
#include <iostream>


BOOST_AUTO_TEST_SUITE(minimal_test)

BOOST_AUTO_TEST_CASE(paragroup_test_all_null)
{
    ff::paragroup pg1;
    ff::ff_wait(all(pg1));
}

BOOST_AUTO_TEST_CASE(paragroup_test_any_null)
{
    ff::paragroup pg1;
    ff::ff_wait(any(pg1));
}

BOOST_AUTO_TEST_CASE(paragroup_empty_test)
{
    std::vector<int> s;
    int sum = 0;
    ff::paragroup pg1;
    pg1.for_each(s.begin(), s.end(), [&sum](int x){
        sum += x;
    });
    ff::ff_wait(all(pg1));
    BOOST_CHECK(sum == 0);
}

BOOST_AUTO_TEST_CASE(paragroup_for_each_test)
{
    std::vector<int> s;
    s.push_back(10);
    s.push_back(11);
    s.push_back(15);
    s.push_back(9);

    int ssum = 0;
    std::for_each(s.begin(), s.end(), [&ssum](int x){ssum += x;});

    ff::accumulator<int> sum(0, [](const int & x, const int& y){return x + y;});
    ff::paragroup pg1;
    pg1.for_each(s.begin(), s.end(), [&sum](int x){
       sum.increase(x);
    });
    ff::ff_wait(all(pg1));

    BOOST_CHECK(ssum == sum.get());
}

BOOST_AUTO_TEST_CASE(paragroup_for_each_large_test)
{
    std::vector<int> s;
    for(int i = 0; i < 10000; i ++)
      s.push_back(i);

    int ssum = 0;
    std::for_each(s.begin(), s.end(), [&ssum](int x){ssum += x;});

    ff::accumulator<int> sum(0, [](const int & x, const int& y){return x + y;});
    ff::paragroup pg1;
    pg1.for_each(s.begin(), s.end(), [&sum](int x){
       sum.increase(x);
    });
    ff::ff_wait(all(pg1));

    BOOST_CHECK_MESSAGE(ssum == sum.get(), "sum is " <<sum.get()<<", should be "<<ssum);
}

BOOST_AUTO_TEST_CASE(paragroup_for_each_search_test)
{
    std::vector<int> s;
    for(int i = 0; i < 10000; i ++)
      s.push_back(i);
    ff::single_assign<int> pos;
    int to_search = 983;
    pos = to_search;
    ff::paragroup pg1;
    pg1.for_each(s.begin(), s.end(), [&pos, to_search](int x){
                pos = to_search;});
    ff::ff_wait(all(pg1));
    BOOST_CHECK(pos.get() == to_search);
}

BOOST_AUTO_TEST_CASE(paragroup_for_each_any_test)
{
    std::vector<int> s;
    for(int i = 0; i < 10000; i ++)
      s.push_back(i);

    int ssum = 0;
    std::for_each(s.begin(), s.end(), [&ssum](int x){ssum += x;});

    ff::accumulator<int> psum(0, [](const int & x, const int& y){return x + y;});
    ff::paragroup pg1;
    pg1.for_each(s.begin(), s.end(), [&psum](int x){
       psum.increase(x);
    });
    ff::ff_wait(any(pg1));

    BOOST_CHECK(psum.get() > 0);
    BOOST_CHECK(psum.get() <= ssum);
}
    

BOOST_AUTO_TEST_SUITE_END()
