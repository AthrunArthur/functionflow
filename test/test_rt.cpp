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
#include <iostream>
#include <vector>
#include <algorithm>
#include "ff.h"
#include "common/log.h"

using namespace ff;
#define FF_TEST_TIME 111


int inc(int t)
{
    return t + 1;
}
double inc(double t)
{
    return t + 1;
}

/*
int fib(int n)
{
	if(n <=2)
		return 1;
	ff::para<int> a, b;
	a([&n]()->int{return fib(n - 1);});
	b([&n]()->int{return fib(n - 2);});
	return (a && b).then([](int x, int y){return x + y;});
}*/

#define BOOST_CHECK(a) if(!(a)) {\
  _DEBUG(std::cout<<"error\t"<<#a<<std::endl;)}

#define BOOST_CHECK_MESSAGE(a, b) if(!(a)) {\
  _DEBUG(std::cout<<"error\t"<<#a<< b<<std::endl;)}


void ff_test_para()
{
    int num = 10;
    
    para<int> a;
    a([&num]() {
        return inc(num);
    }).then([&num](int x) {
        BOOST_CHECK(x == inc(num));
    });
    ff::para<> b;
    int b_res;
    b[a]([&num, &a, &b_res]() {
        b_res = inc(num + a.get());
    }).then([&num, &a, &b, &b_res]() {
        BOOST_CHECK(b_res == inc(num + a.get()));
    });

    ff::para<int> f1;
    ff::para<double> f2;
    f1([]() {
        return inc(1);
    });
    f2([]() {
        return inc(2.2);
    });
    (f1 || f2).then([](int index, std::tuple<int, double> res) {
        if(index == 0)
        {   BOOST_CHECK_MESSAGE(std::get<0>(res) == inc(1), "res:"<<std::get<0>(res)<<", should be "<<inc(1));
        }
        else if(index == 1)
        {   BOOST_CHECK_MESSAGE(std::get<1>(res) == inc(2.2), "res:"<<std::get<1>(res)<<", should be "<<inc(2.2));
        }
    });

    ff::para<> f3;
    f3([]() {
        return inc(3);
    });
    ff::para<double> f4;
    f4[f3]([]() {
        return inc(4.5);
    }).then([](double x) {
        BOOST_CHECK(x == inc(4.5));
    });
    (f3 && f4).then([](double x) {
        BOOST_CHECK(x == inc(4.5));
    });
}

void ff_test_paragroup()
{
    std::vector<int> s;
    s.push_back(10);
    s.push_back(11);
    s.push_back(15);
    s.push_back(9);

    int ssum = 0;
    std::for_each(s.begin(), s.end(), [&ssum](int x) {
        ssum += x;
    });

    ff::accumulator<int> sum(0, [](const int & x, const int& y) {
        return x + y;
    });
    ff::paragroup pg1;
    pg1.for_each(s.begin(), s.end(), [&sum](int x) {
        sum.increase(x);
    });
    ff_wait(all(pg1));

    BOOST_CHECK(ssum == sum.get());
    //std::cout<<"fff"<<std::endl;
    /*
    ff::paragroup pg2;
    ff::single_assign<int> first;
    pg2.for_each(s.begin(), s.end(), [&first](int x){first = x;});
    std::cout<<"pg2 first: "<<first.get()<<std::endl;
    */
}

void ff_test_basic()
{
  para<int> a, b;
  ff_wait(a);
  std::cout<<"test wait null task ok"<<std::endl;

  paragroup pg;
  ff_wait(all(pg));
  std::cout<<"test wait null paragroup ok"<<std::endl;

  std::vector<int> s;
  paragroup pg1;
  pg1.for_each(s.begin(), s.end(), [](int p){p ++;});
  ff_wait(all(pg1));
  std::cout<<"test wait paragroup with null tasks ok"<<std::endl;

  paragroup pg2;
  s.push_back(1);
  pg2.for_each(s.begin(), s.end(), [](int p){p ++;});
  ff_wait(all(pg2));
  std::cout<<"test wait paragroup with 1 task ok"<<std::endl;
}


int main(int argc, char *argv[])
{
    _DEBUG(ff::fflog<>::init(ff::TRACE, "log.txt"))
    _DEBUG(LOG_INFO(main)<<"main start, id:"<<ff::rt::get_thrd_id());

    
    for(int i = 0; i < FF_TEST_TIME; ++i)
    {
        ff_test_para();
        ff_test_paragroup();
    }
    std::cout<<"test_para ok"<<std::endl;
    std::cout<<"test_paragroup ok"<<std::endl;


    ff_test_basic();
    std::cout<<"test_wait ok"<<std::endl;
    return 0;
}
