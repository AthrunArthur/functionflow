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
#include <iostream>
#include "utilities/simo_queue.h"
#include <thread>

using namespace ff;
using namespace ff::rt;

typedef simo_queue<int64_t, 12> MQ_t;
const static int NUM = (1<<12) - 1;

BOOST_AUTO_TEST_SUITE(minimal_test)
BOOST_AUTO_TEST_CASE(base_test)
{
  MQ_t q;
  bool b;
  for(int64_t i = 0; i < NUM; ++i)
  {
    b = q.push(i);
    BOOST_CHECK(b);
  }
  b = q.push(0);
  BOOST_CHECK(!b);
  for(int i = 0; i < NUM; ++i)
  {
    int64_t t = 0;
    b = q.pop(t);
    BOOST_CHECK(b);
    BOOST_CHECK(t == i);
  }
  int64_t t = 0;
  b = q.pop(t);
  BOOST_CHECK(!b);
}

std::atomic_size_t input_counter(0);
std::atomic_llong input_sum(0);
std::atomic_size_t output_counter(0);

void push_thread_func(int64_t & sum, MQ_t & q)
{
  sum = 0;
  for(int i = 0; i < NUM; i++)
  {
    if(q.push(i))
    {
      input_counter ++;
      sum += i;
      input_sum += i;
    }
    else
      std::this_thread::yield();
  }
}

int64_t pop_thread_func(std::atomic_bool & stop_flag, MQ_t & q)
{
  int64_t sum = 0;
  while(!stop_flag || q.size() != 0)
  {
    int64_t t = 0;
    while(q.pop(t))
    {
      sum += t;
      output_counter ++;
    }
    std::this_thread::yield();
  }
  return sum;
}

BOOST_AUTO_TEST_CASE(full_test)
{
  std::atomic_bool is_stopped(false);
  std::atomic_bool is_start(false);
 typedef std::shared_ptr<std::thread> thread_ptr;
  std::vector<thread_ptr> thrds;
  std::vector<int64_t > sums;
  int64_t total_sum = 0;
  MQ_t q;
 int64_t total_input_sum = 0;
 std::thread push_thread([&total_input_sum, &q, &is_start](){
     while(!is_start) std::this_thread::yield();
     push_thread_func(total_input_sum, q);
     });
 for(int i = 0; i < std::thread::hardware_concurrency(); i ++)
 {
   sums.push_back(0);
 }
 for(int i = 0; i < std::thread::hardware_concurrency(); i ++)
 {
   thrds.push_back(thread_ptr(new std::thread([&sums, &is_stopped, i, &q](){
           sums[i] = pop_thread_func(is_stopped, q);
           })));
 }
 is_start = true;
 push_thread.join();
 is_stopped = true;
 for(int i = 0; i < thrds.size(); ++i)
   thrds[i]->join();
 std::cout<<"check in "<<input_counter.load()<<" items"<<std::endl;
 std::cout<<"check in  sum "<<input_sum.load()<<std::endl;

 int64_t total_output_sum = 0;
 for(int i = 0; i < thrds.size(); ++i)
 {
   total_output_sum += sums[i];
 }
 BOOST_CHECK_MESSAGE(input_counter.load() == output_counter.load(), "output counter is "<<output_counter.load()<<" should be "<<input_counter.load());
 BOOST_CHECK_MESSAGE(total_output_sum == total_input_sum, "input sum is "<<total_input_sum<<", output sum is "<<total_output_sum);
}
BOOST_AUTO_TEST_SUITE_END()
