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
#include <gtest/gtest.h>

void c_func1(){}

void c_func2(int a){}

auto cl1 = [](){return 1;};
auto cl2 = std::function<void(int)>(c_func2);

struct MF1{
  void operator ()(){}
};
struct MF2{
  void operator ()(int a){}
};

struct MF3{
  void ff(){}
};


TEST(TestCallable, NormalCase)
{
  bool b = ff::utils::is_callable<decltype(c_func1)>::value;
  EXPECT_TRUE(b == true);
  b = ff::utils::is_callable<decltype(c_func2)>::value;
  EXPECT_TRUE(b == true);
  int c;
  b = ff::utils::is_callable<decltype(c)>::value;
  EXPECT_TRUE(b == false);
}

TEST(TestCallable, CPP11Case)
{
  bool b;
  b = ff::utils::is_callable<decltype(cl1)>::value;
  EXPECT_TRUE(b == true);

  b = ff::utils::is_callable<decltype(cl2)>::value;
  EXPECT_TRUE(b == true);
}

TEST(TestCallable, FunctorCase)
{
  bool b;
  b = ff::utils::is_callable<MF1>::value;
  EXPECT_TRUE(b == true);

  b = ff::utils::is_callable<MF2>::value;
  EXPECT_TRUE(b == true);

  b = ff::utils::is_callable<MF3>::value;
  EXPECT_TRUE(b == false);
}

template<class F>
void checker(F && f, bool req)
{
    bool b = ff::utils::is_callable<F>::value;
    EXPECT_TRUE(b == req);
}
TEST(TestCallable, FuncPassCase)
{
  checker(c_func1, true);
  checker(c_func2, true);
}

TEST(TestCallable, CPP11Pass)
{
  checker([](){}, true);
  checker(cl1, true);
  checker(cl2, true);
}

TEST(TestCallable, ObjPass)
{
  MF1 a;
  checker(a, true);
  MF3 b;
  checker(b, false);
}

