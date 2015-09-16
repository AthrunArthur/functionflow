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

//#include "runtime/gtwsq_fixed.h"
#include "runtime/work_stealing_queue.h"
#include <cstdlib>
#include <thread>
#include <gtest/gtest.h>

using namespace ff;
using namespace ff::rt;
//typedef gcc_work_stealing_queue<int64_t, 8> MQ_t;
typedef cpp_work_stealing_queue<int64_t, 8> MQ_t;
const static int NUM = (1<<8) - 1;

TEST(WSQ, Basic)
{
  MQ_t q;
  bool b;
  for(int i = 0; i < NUM; ++i)
  {
    b = q.push_back(i);
    EXPECT_TRUE(b);
  }
  b = q.push_back(0);
  EXPECT_TRUE(!b);
  int64_t t = 0;
  for(int i = NUM-1; i >=0 ; --i)
  {
    b = q.pop(t);
    EXPECT_TRUE(b);
    EXPECT_TRUE(t == i);
  }
  b = q.pop(t);
  EXPECT_TRUE(!b);
}

class WSQTest : public ::testing::Test{
  protected:
    virtual void SetUp() {
      input_counter.store(0);
      input_sum.store(0);
      output_counter.store(0);
      g_start_flag.store(false);
      g_end_flag.store(false);
    }
    int64_t work_thread_func(MQ_t & q)
    {
      int64_t sum = 0;
      while(!g_start_flag) std::this_thread::yield();
      int64_t op_nums = NUM*256*256;
      for(int64_t i = 0; i < op_nums; ++i)
      {
        if(rand()% 3 != 0)
        {
          int64_t n = rand()%(i+1);
          if(q.push_back(n)){
            input_counter ++;
            input_sum += n;
          }
        }
        else
        {
          int64_t n = 0;
          if(q.pop(n))
          {
            if(n == -1)
              std::cout<<"pop wrong number! "<<q.size()<<std::endl;
            output_counter ++;
            sum += n;
          }
        }
      }
      g_end_flag = true;
      return sum;
    }
    int64_t thief_thread_func(MQ_t & q)
    {
      int64_t sum = 0;
      while(!g_start_flag) std::this_thread::yield();
      while(!g_end_flag || q.size() != 0)
      {
        int64_t t  = 0;
        while(q.steal(t))
        {
          if(t == -1)
          {
            std::cout<<"steal wrong number"<<q.size()<<std::endl;
          }
          sum += t;
          output_counter ++;
        }
        std::this_thread::yield();
      }
      return sum;
    }
    std::atomic_size_t input_counter;
    std::atomic_llong input_sum;
    std::atomic_size_t output_counter;
    std::atomic_bool g_start_flag;
    std::atomic_bool g_end_flag;
};

TEST_F(WSQTest,FullTest)
{
  typedef std::shared_ptr<std::thread> thread_ptr;
  std::vector<thread_ptr> thrds;
  std::vector<int64_t > sums;
  int THRD_NUM = 5;
  MQ_t q;
  for(int i = 0; i < THRD_NUM; ++i)
  {
    sums.push_back(0);
  }
  thrds.push_back(thread_ptr(new std::thread([&sums, &q, this](){
          sums[0] = work_thread_func(q);
          })));
  for(int i = 1; i < THRD_NUM; ++i)
  {
    thrds.push_back(thread_ptr(new std::thread([&sums, i, &q, this](){
            sums[i] = thief_thread_func(q);
            })));
  }

  g_start_flag = true;
  int64_t total_output_sum = 0;
  for(int i = 0; i < thrds.size(); ++i)
  {
    thrds[i]->join();
    total_output_sum += sums[i];
  }
  EXPECT_TRUE(input_counter.load() == output_counter.load())<< "input "<<input_counter.load()<<" items, out "<< output_counter.load();
  EXPECT_TRUE(input_sum.load() == total_output_sum)<< "input_sum is "<<input_sum.load()<<", while output sum is "<<total_output_sum;
}
