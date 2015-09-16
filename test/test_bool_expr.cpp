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
#include <gtest/gtest.h>
#include <iostream>
#include <vector>
#include <algorithm>
#include "ff.h"

using namespace ff;
#define FF_TEST_TIME 100



int inc_b(int t)
{
	return t + 1;
}
double inc_b(double t)
{
	return t + 1;
}

TEST(TestBoolExpr,BoolOrExpr)
{
	ff::para<int> f1;
	ff::para<double> f2;
	f1([](){return inc_b(1);});
	f2([](){return inc_b(2.2);});
	(f1 || f2).then([](int index, std::tuple<int, double> res){
		if(index == 0)
			EXPECT_TRUE(std::get<0>(res) == inc_b(1))<< "res:"<<std::get<0>(res)<<", should be "<<inc_b(1);
		else if(index == 1)
			EXPECT_TRUE(std::get<1>(res) == inc_b(2.2))<< "res:"<<std::get<1>(res)<<", should be "<<inc_b(2.2);
	});
}

TEST(TestBoolExpr,BoolAndExpr)
{
	ff::para<> f3;
	f3([](){inc_b(3);});
	ff::para<double> f4;
	f4[f3]([](){return inc_b(4.5);}).then([](double x){
		EXPECT_TRUE(x == inc_b(4.5));
	});
	ff_wait(f4);
	(f3 && f4).then([](double x){
		EXPECT_TRUE(x == inc_b(4.5));
	});
}

TEST(TestBoolExpr, BoolAndExprParen)
{
  ff::para<int> f1;
  ff::para<double> f2;
  f1([](){return inc_b(1);});
  f2([](){return inc_b(2.2);});
  ff::para<> f3;
  f3[f1 || f2]([](int index, std::tuple<int, double> res){
      if(index == 0)
        EXPECT_TRUE(std::get<0>(res) == inc_b(1))<< "res:"<<std::get<0>(res)<<", should be "<<inc_b(1);
      else if(index == 1)
        EXPECT_TRUE(std::get<1>(res) == inc_b(2.2))<< "res:"<<std::get<1>(res)<<", should be "<<inc_b(2.2);
      });
  ff::para<double> f4;
  f4[f1 || f2]([](int index, std::tuple<int, double> res)->double{
      if(index == 0)
        EXPECT_TRUE(std::get<0>(res) == inc_b(1))<< "res:"<<std::get<0>(res)<<", should be "<<inc_b(1);
      else if(index == 1)
        EXPECT_TRUE(std::get<1>(res) == inc_b(2.2))<< "res:"<<std::get<1>(res)<<", should be "<<inc_b(2.2);
      return inc_b(2.2);
      });
  ff_wait(f3 && f4);
  ff_wait(f1 && f2);
}

TEST(TestBoolExpr, BoolAndExprParenFirstVoid)
{
  ff::para<int> f1;
  ff::para<void> f2;
  f1([](){return inc_b(1);});
  f2([](){inc_b(2.2);});

  ff::para<> f3;
  f3[f1 || f2]([](bool valid_flag, int r){
      if(valid_flag)
        EXPECT_TRUE(r == inc_b(1));
      });
  ff::para<double> f4;
  f4[f1 || f2]([](bool valid_flag, int r){
      if(valid_flag)
        EXPECT_TRUE(r == inc_b(1));
      return inc_b(2.2);
      });
  ff::para<> f5;
  f5[f1 || f2]([](bool f, int r1){
      //we do nothing here, just make sure it compiles and run!
      });

  ff::para<> f6;
  f6[f1 || f2]([](bool f, int r1){
      //we do nothing here, just make sure it compiles and run!
      });
  ff_wait(f3 && f4);
  ff_wait(f1 && f2 && f5 && f6);
}

TEST(TestBoolExpr, BoolAndExprParenSecondVoid)
{
  ff::para<int> f2;
  ff::para<void> f1;
  f2([](){return inc_b(1);});
  f1([](){inc_b(2.2);});

  ff::para<> f3;
  f3[f1 || f2]([](bool valid_flag, int r){
      if(valid_flag)
        EXPECT_TRUE(r == inc_b(1));
      });
  ff::para<double> f4;
  f4[f1 || f2]([](bool valid_flag, int r){
      if(valid_flag)
        EXPECT_TRUE(r == inc_b(1));
      return inc_b(2.2);
      });
  ff::para<> f5;
  f5[f1 || f2]([](bool vf, int r){
      //we do nothing here, just make sure it compiles and run!
      });

  ff::para<> f6;
  f6[f1 || f2]([](bool vf, int r){
      //we do nothing here, just make sure it compiles and run!
      });
  ff_wait(f3 && f4);
  ff_wait(f1 && f2 && f5 && f6);
}
