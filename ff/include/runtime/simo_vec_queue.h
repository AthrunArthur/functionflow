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
#ifndef FF_RUNTIME_SIMO_VEC_QUEUE_H_
#define FF_RUNTIME_SIMO_VEC_QUEUE_H_

#include "common/common.h"
#include "runtime/rtcmn.h"
#include "common/spin_lock.h"
#include <vector>

namespace ff{
  namespace rt{
    //N, 2^N
    //! This queue is for single-thread's push, and multiple-threads' pop.
    //! This queue is capability-fixed
    template<class T, size_t N, size_t VEC_SIZE = 256>
      class simo_vec_queue{
        const static int64_t MASK = (1<<N) - 1;
        const static size_t VEC_INIT_SIZE = VEC_SIZE;
        typedef std::vector<T> vec_t;
        public:
        simo_vec_queue(size_t thrd_num)
          :array(nullptr),cap(0), head(0), tail(0)
           , vec_size(VEC_INIT_SIZE), threshold(thrd_num){
             array=new vec_t[1<<N];
             cap = 1<<N;
           }

        bool push(const T & val)
        {
          if(head - tail >= MASK)
            return false;
          vec_t & m = array[head & MASK];
          m.push_back(val);
          if(m.size() >= vec_size)
          {
            head ++;
            //vec_size = (((head-tail)/threshold) + 1 )* VEC_INIT_SIZE;
          }
          return true;
        }
        //!call this only when there won't be any "push" operation anymore!
        bool pop_till_clear(T & val)
        {
          vec_t & m = array[head & MASK];
          mutex.lock();
          if(m.empty()){
            mutex.unlock();
            return false;
          }
          val = m.back();
          m.pop_back();
          mutex.unlock();
          return true;
        }
        bool pop(std::vector<T> & vals)
        {
            auto t = tail;
            if(t == head) return false;

            vals = array[t&MASK];
            while(!__sync_bool_compare_and_swap(&tail, t, t+1))
            {
              t = tail;
              vals.clear();
              if(t == head) return false;
              vals = array[t&MASK];
            }
            return true;
        }
        bool pop(T & val){
          if(s_vec.empty()){
            auto t = tail;
            if(t == head) return false;

            s_vec = array[t&MASK];
            while(!__sync_bool_compare_and_swap(&tail, t, t+1))
            {
              t = tail;
              s_vec.clear();
              if(t == head) return false;
              s_vec = array[t&MASK];
            }
          }
          if(!s_vec.empty())
          {
            val = s_vec.back();
            s_vec.pop_back();
            return true;
          }
          return false;
        }
        size_t size() const {return head - tail;}

        protected:
        vec_t * array;
        int64_t cap;
        int64_t head;
        int64_t tail;
        size_t vec_size;
        size_t threshold;
        thread_local static vec_t s_vec;
        ff::spinlock mutex;
      };//end class simo_queue;

    template<typename T, size_t N, size_t VEC_SIZE>
    thread_local std::vector<T> simo_vec_queue<T, N, VEC_SIZE>::s_vec;
  }
}//end namespace ff
#endif
