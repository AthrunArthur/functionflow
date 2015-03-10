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

#ifndef FF_RUNTIME_GTWSQ_FIXED_H_
#define FF_RUNTIME_GTWSQ_FIXED_H_
#include "common/common.h"
#include "runtime/rtcmn.h"
#include "common/log.h"
#include "common/scope_guard.h"
#include "common/spin_lock.h"
#include "common/thread_local_storage.h"

#ifdef FUNCTION_FLOW_DEBUG
#include "runtime/record.h"
#endif

namespace ff{
namespace rt{

//N, 2^N.
template <class T, size_t N>
class gcc_work_stealing_queue
{
    const static int64_t INITIAL_SIZE=1<<N;
public:
    gcc_work_stealing_queue()
      : rflag(0)
      , head(0)
      , tail(0)
      , cap(1<<N)
      , array(new T[1<<N])
        {
        if(array == nullptr)
        {
          assert(false && "Allocation Failed!");
          exit(-1);
        }
      }

    ~gcc_work_stealing_queue()
    {
      //std::cout<<p1<<", "<<p2<<", "<<p3<<", "<<p4<<std::endl;
      if(array != nullptr)
      {
        delete[] array;
        array = nullptr;
      }
    }

    bool push_back(const T & val)
    {
      auto t = tail;
      auto h = head;
      if(t - h == cap - 1)
      {
        return false;
      }
      auto mask = cap - 1;
      array[t&mask] = val;
      tail = t + 1;
      return true;
    }

    bool pop(T & val)
    {
      int64_t t = tail;
      t = t - 1;
      tail = t;
      int64_t h = head;
      int s = t-h;
      if (s < 0)
      {
        tail = h;
        return false;
      }
      auto mask = cap - 1;
      val = array[t&mask];
      if(s > 0)
      {
        return true;
      }
      bool res = true; 
      if(!__sync_bool_compare_and_swap(&head, h, h+1))
      {
        res = false;
      }
      tail = h + 1;
      return res;
    }

    bool steal(T & val)
    {
      int64_t h = head;
      int64_t t = tail;

      int s = t - h;
      if ( s <= 0){
        return false;
      }
      auto mask = cap - 1;
      val = array[h&mask];
      if(__sync_bool_compare_and_swap(&head, h, h+1))
      {
        return true;
      }
      return false;
    }

    uint64_t size()
    {
      return static_cast<uint64_t>(tail - head);
    }
    int64_t  get_head() const {return head;}
    int64_t  get_tail() const {return tail;}
protected:
    int16_t rflag;
    int64_t head;
    int64_t tail;
    int64_t   cap;
    T *       array;
};//end class classical_work_stealing_queue
#undef MEM_SEQ_CST
#undef MEM_ACQUIRE
#undef MEM_RELAXED
#undef MEM_RELEASE

}//end namespace rt
}//end namespace ff
#endif

