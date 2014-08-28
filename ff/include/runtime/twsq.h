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

#ifndef FF_RUNTIME_TWSQ_H_
#define FF_RUNTIME_TWSQ_H_
#include "common/common.h"
#include "runtime/rtcmn.h"
#include "common/log.h"
#include "common/scope_guard.h"

namespace ff{
namespace rt{
#define MEM_SEQ_CST std::memory_order_seq_cst
  /*
#define MEM_ACQUIRE std::memory_order_seq_cst
#define MEM_RELAXED std::memory_order_seq_cst
#define MEM_RELEASE std::memory_order_seq_cst
*/

#define MEM_ACQUIRE std::memory_order_acquire
#define MEM_RELAXED std::memory_order_relaxed
#define MEM_RELEASE std::memory_order_release

//N, 2^N.
template <class T, size_t N>
class classical_work_stealing_queue
{
    const static int64_t INITIAL_SIZE=1<<N;
public:
    classical_work_stealing_queue()
      : thieves(0)
      , is_resizing(false)
      , head(0)
      , tail(0)
      , cap(1<<N)
      , array(new T[1<<N]){
        if(array == nullptr)
        {
          assert(false && "Allocation Failed!");
          exit(-1);
        }
      }

    ~classical_work_stealing_queue()
    {
      if(array != nullptr)
      {
        delete[] array;
        array = nullptr;
      }
    }

    void push_back(const T & val)
    {
      auto t = tail.load(MEM_ACQUIRE);
      auto h = head.load(MEM_ACQUIRE);
      if(t - h == cap - 1)
      {
        resize(cap<<1, h, t);
      }
      auto mask = cap - 1;
      array[t&mask] = val;
      tail.store(t+1, MEM_RELEASE);
    }

    bool pop(T & val)
    {
      auto t = tail.load(MEM_ACQUIRE);
      t = t - 1;
      tail.store(t, MEM_RELEASE);
      auto h = head.load(MEM_ACQUIRE);
      auto s = t-h;
      if (s < 0)
      {
        tail.store(h, MEM_RELEASE);
        return false;
      }
      auto mask = cap - 1;
      val = array[t&mask];
      if(s > 0)
      {
        if(s <= cap >> 2 && 
           s > INITIAL_SIZE)
        {
          resize(cap >> 1, h, t);
        }
        return true;
      }
      bool res = true; 
      if(!head.compare_exchange_strong(h, h+1, std::memory_order_acq_rel))
      {
        res = false;
      }
      tail.store(h + 1, MEM_RELEASE);
      return res;
    }

    bool steal(T & val)
    {
      while(is_resizing.load(MEM_ACQUIRE)) yield();
      scope_guard _sg([this](){
          thieves ++;
      }, [this](){
          thieves --;
      });
      auto t = tail.load(MEM_ACQUIRE);
      auto h = head.load(MEM_ACQUIRE);
      auto s = t - h;
      if ( s <= 0)
        return false;
      auto mask = cap - 1;
      val = array[h&mask];
      if(head.compare_exchange_strong(h, h + 1, std::memory_order_acq_rel))
      {
        return true;
      }
      return false;
    }

    uint64_t size()
    {
      return static_cast<uint64_t>(tail.load(MEM_ACQUIRE) - head.load(MEM_ACQUIRE));
    }

    void resize(int64_t s, int64_t h, int64_t t)
    {
      auto c1 = new T[s];
      if(c1 == nullptr)
      {
        assert(false && "Allocation failed");
        exit(-1);
      }
      auto mask = cap - 1;
      auto m1 = s - 1;
      for(int64_t i = h; i < t; ++i)
      {
        c1[i&m1] = array[i&mask];
      }
      is_resizing.store(true, MEM_RELEASE);
      while(thieves.load(MEM_ACQUIRE) != 0) yield();
      cap = s;
      auto tp = array;
      array = c1;
      is_resizing.store(false, MEM_RELEASE);
      delete[] tp;
    }
protected:
    std::atomic<int>    thieves;
    std::atomic<bool>   is_resizing;
    std::atomic<int64_t>   head;
    std::atomic<int64_t>   tail;
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
