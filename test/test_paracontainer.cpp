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

using namespace ff;

TEST(ParaContainer, AllNull)
{
  ff::paracontainer pc;
  ff::ff_wait(all(pc));
}

TEST(ParaContainer, AnyNull)
{
  ff::paracontainer pc;
  ff::ff_wait(any(pc));
}
TEST(ParaContainer, Empty)
{
  ff::paracontainer pc;
  ff::paracontainer pc2;
  ff::para<> a;
  int cached_exceptions = 0;
  pc.add(a);
  pc2.add(a);
  try{
    ff::ff_wait(all(pc));
  }
  catch(empty_para_exception & e){
    cached_exceptions ++;
  }
  try{
    ff::ff_wait(all(pc2));
  }
  catch(empty_para_exception & e){
    cached_exceptions ++;
  }
}

TEST(ParaContainer, TestSum)
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
  EXPECT_TRUE(para_sum.load() == require_sum);
}

TEST(ParaContainer, TestAny)
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
  ff::ff_wait(any(pc));
  EXPECT_TRUE(para_sum.load() != 0);
}

TEST(ParaContainer, TestStressedAny)
{

  const static int max = 10000;
  for(int c = 0; c < max; ++c){
    std::atomic_int para_sum(0);
    ff::paracontainer pc;
    for(int i = 1; i < 3; ++i)
    {
      ff::para<> a;
      a([i, &para_sum](){para_sum += i;});
      pc.add(a);
    }
    ff::ff_wait(any(pc));
    EXPECT_TRUE(para_sum.load() != 0);

  }
}
