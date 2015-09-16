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
#include <functional>
#include "ff.h"
#include <gtest/gtest.h>

using namespace ff;
#define FF_TEST_TIME 100




int inc(int t)
{
	return t + 1;
}
double inc(double t)
{
	return t + 1;
}


TEST(Interface, Simple)
{
    para<> a;
    para<int> b;
    a([](){});
    b([](){return 10;});
    ff_wait(a);
    ff_wait(b);
    EXPECT_TRUE(b.get() == 10);
    EXPECT_TRUE(a.check_if_over());
    EXPECT_TRUE(b.check_if_over());
}

int intinc(int t)
{
  return t + 1;
}
int incone()
{
  return 10;
}
class IncOne
{
public:
  IncOne(int & i):
    m_i(i){}
  int get(){return m_i;}
  int operator()()
  {
    return m_i + 1;
  }
protected:
  int & m_i;
};//end class IncOne

TEST(Interface, FunctionSimple)
{
  para<int> b;
  b(incone);
  ff_wait(b);
  EXPECT_TRUE(b.get() == 10);
  EXPECT_TRUE(b.check_if_over());
}

TEST(Interface, FunctionBind)
{
  para<int> b;
  int a = 9;
  b(std::bind(intinc, a));
  ff_wait(b);
  EXPECT_TRUE(b.get() == 10);
  EXPECT_TRUE(b.check_if_over());
}

TEST(Interface, Functor)
{
  para<int> b;
  int t = 9;
  IncOne v(t);
  b(v);
  ff_wait(b);
  EXPECT_TRUE(b.get() == 10);
  EXPECT_TRUE(b.check_if_over());
}
TEST(Interface, Then)
{
    int num = 10;
    para<int> a;
    a([&num](){return inc(num);}).then([num](int n){EXPECT_TRUE(n == inc(num));});
    EXPECT_TRUE(a.check_if_over());
}

TEST(Interface, ThenSimple)
{
    int num = 10;
    para<int> a;
    int t = a([&num](){return inc(num);}).then(intinc);
    EXPECT_TRUE(t == intinc(inc(num)));
}
struct IntFunctor
{
  int operator()(int n){return n + 1;}
};//end class IntFunctor
TEST(Interface, ThenFunctor)
{
  int num = 10;
  para<int> a;
  IntFunctor ftr;
  int t = a([&num](){return inc(num);}).then(ftr);
  EXPECT_TRUE(t == inc(num) + 1);
}

int add(int a, int b)
{
  return a + b;
}
TEST(Interface, ThenBind)
{
  int num = 10;
  para<int> a;
  int t = a([&num](){return inc(num);}).then(std::bind(add, std::placeholders::_1, 10));
  EXPECT_TRUE(t == add(10, inc(num)));
}

TEST(Interface, Empty)
{
  int num = 10;
  double b_res;
  para<int> a;
  para<> b;
  int cached_exceptions = 0;

  try{
      b[a]([&num, &b_res](int ret){b_res=inc(num + ret);}).then([&num, &a, &b, &b_res](){
        EXPECT_TRUE(b_res == inc(num + a.get()));
      });}
  catch(empty_para_exception & e){
     cached_exceptions ++;
  }
  EXPECT_EQ(cached_exceptions, 1);
  try{
    ff_wait(b);
  }
  catch(empty_para_exception & e){
     cached_exceptions ++;
  }
  EXPECT_EQ(cached_exceptions, 2);

  try{
    ff_wait(a);
  }
  catch(empty_para_exception & e){
     cached_exceptions ++;
  }
  EXPECT_EQ(cached_exceptions, 3);
  EXPECT_TRUE(!a.check_if_over());
  EXPECT_TRUE(!b.check_if_over());
}

TEST(Interface, Para)
{
    int num = 10;
    para<int> a;
    a([&num](){return inc(num);}).then([&num](int x){
        EXPECT_TRUE(x == inc(num));
    });
    ff::para<> b;
    int b_res;
    b[a]([&num, &b_res](int ret){b_res = inc(num + ret);}).then([&num, &a, &b, &b_res](){
        EXPECT_TRUE(b_res == inc(num + a.get()));
    });
}
TEST(Interface, ParaDep)
{
  int num = 10;
  for(int i = 0; i < 100; ++i)
  {
    para<int> a;
    a([&num](){return num;});
    para<void> b;
    b[a]([num](int ret){EXPECT_TRUE(ret == num);});
    ff_wait(b&&a);
  }
}
TEST(Interface, ParaDepAnd)
{
    paracontainer pc;
    for(int i = 0; i < 1000; ++i)
    {
        para<int> a, b;
        para<> c;
        a([i](){return i;});
        b([i](){return i*i;});
        c[a&&b]([](int t1, int t2){
            EXPECT_TRUE(t2 == t1*t1);
        });
        pc.add(c);
    }
    ff_wait(all(pc));
}

