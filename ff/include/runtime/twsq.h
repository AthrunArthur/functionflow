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
#include "common/spin_lock.h"

#ifdef FUNCTION_FLOW_DEBUG
#include "runtime/record.h"
#endif

namespace ff{
namespace rt{
  /*
#define MEM_SEQ_CST
#define MEM_ACQUIRE
#define MEM_RELAXED
#define MEM_RELEASE
*/
#define MEM_SEQ_CST std::memory_order_seq_cst
#define MEM_ACQUIRE std::memory_order_seq_cst
#define MEM_RELAXED std::memory_order_seq_cst
#define MEM_RELEASE std::memory_order_seq_cst

/*
#define MEM_ACQUIRE std::memory_order_acquire
#define MEM_RELAXED std::memory_order_relaxed
#define MEM_RELEASE std::memory_order_release
*/

//N, 2^N.
template <class T, size_t N>
class classical_work_stealing_queue
{
    const static int64_t INITIAL_SIZE=1<<N;
public:
    classical_work_stealing_queue()
      : rflag(0)
      , head(0)
      , tail(0)
      , cap(1<<N)
#ifdef FUNCTION_FLOW_DEBUG
      , m_rid(-1)
#endif
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
#ifdef FUNCTION_FLOW_DEBUG
      thread_local static thrd_id_t id = get_thrd_id();
      m_id = id;
      m_rid ++;
#endif
      auto t = tail.load();
      auto h = head.load();
      if(t - h == cap - 1)
      {
        resize(cap<<1, h, t);
      }
#ifdef FUNCTION_FLOW_DEBUG
      record r(m_rid.load(), m_id, id, record::op_push, h, t);
      all_records::getInstance()->add(r);
#endif
      auto mask = cap - 1;
      array[t&mask] = val;
      tail.store(t+1);
    }

    bool pop(T & val)
    {
      int64_t t = tail.load();
      t = t - 1;
      tail.store(t);
      int64_t h = head.load();
      int s = t-h;
#ifdef FUNCTION_FLOW_DEBUG
      if (t<h)
        assert(s<0 && "xxxxx");
      thread_local static thrd_id_t id = get_thrd_id();
      m_rid ++;
      record r(m_rid.load(), m_id, id, record::op_pop, h, t);
      scope_guard _sg([](){}, [&r](){
          all_records::getInstance()->add(r);
          });
#endif
      if (s < 0)
      {
        tail.store(h);
#ifdef FUNCTION_FLOW_DEBUG
        r.op_res = false;
#endif
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
#ifdef FUNCTION_FLOW_DEBUG
        r.op_res = true;
#endif
        val = array[t & mask];
        return true;
      }
      bool res = true; 
      if(!head.compare_exchange_strong(h, h+1))
      {
        res = false;
      }
      tail.store(h + 1);
#ifdef FUNCTION_FLOW_DEBUG
      r.op_res = res;
#endif
      return res;
    }

    bool steal(T & val)
    {
      scope_guard _sg([this](){
          slock.lock();
          rflag++;
          if(rflag == 1)
                rlock.lock();
          slock.unlock();
      }, [this](){
          slock.lock();
          rflag --;
          if(rflag == 0)
                rlock.unlock();
          slock.unlock();
      });
      auto h = head.load();
      auto t = tail.load();
#ifdef FUNCTION_FLOW_DEBUG
      thread_local static thrd_id_t id = get_thrd_id();
      m_rid ++;
      record r(m_rid.load(), m_id, id, record::op_steal, h, t);
      scope_guard _rg([](){}, [&r](){
          all_records::getInstance()->add(r);
          });
#endif

      auto s = t - h;
      if ( s <= 0){
#ifdef FUNCTION_FLOW_DEBUG
        r.op_res = false;
#endif
        return false;
      }
      auto mask = cap - 1;
      val = array[h&mask];
      if(head.compare_exchange_strong(h, h + 1))
      {
#ifdef FUNCTION_FLOW_DEBUG
        r.op_res = true;
        if(val == 0)
        {
          std::cout<<"steal error, with h:"<<h<<", t:"<<t<<std::endl;
        }
#endif
        return true;
      }
#ifdef FUNCTION_FLOW_DEBUG
        r.op_res = false;
#endif
      return false;
    }

    uint64_t size()
    {
      return static_cast<uint64_t>(tail.load() - head.load());
    }

    void resize(int64_t s, int64_t h, int64_t t)
    {
      assert(false &&" no resize");
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
      rlock.lock();
      cap = s;
      auto tp = array;
      array = c1;
      rlock.unlock();
      delete[] tp;
    }
    int64_t  get_head() const { head.load();}
    int64_t  get_tail() const { tail.load();}
protected:
#ifdef FUNCTION_FLOW_DEBUG
    std::atomic<int64_t>   m_rid;
    thrd_id_t   m_id;
#endif
    std::atomic<int16_t>    rflag; //for resizing
    std::atomic<int64_t>   head;
    std::atomic<int64_t>   tail;
    int64_t   cap;
    T *       array;
    spinlock  rlock;
    spinlock  slock;
};//end class classical_work_stealing_queue
#undef MEM_SEQ_CST
#undef MEM_ACQUIRE
#undef MEM_RELAXED
#undef MEM_RELEASE

}//end namespace rt
}//end namespace ff
#endif
