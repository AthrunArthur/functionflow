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
#ifndef FF_RUNTIME_MUTEX_H_
#define FF_RUNTIME_MUTEX_H_
#include "runtime/env.h"
#include "common/common.h"
#include "runtime/runtime.h"
#include <fstream>
#include <sstream>
#ifdef __linux__
#include <time.h>
#endif

namespace ff {
static int gmcounter = 0;

class mutex
{
public:
    mutex(): m_mutex()
    , m_locked()
    , m_thread_conflicts(rt::allocate_thread_local<bool>())
    , m_thread_schedule_cost(rt::allocate_thread_local<double>(1))
    , callback_prelock([](mutex_id_t){})
    , callback_postlock([](mutex_id_t){})
    , callback_postunlock([](mutex_id_t){}) {};
    
    mutex(const mutex &) = delete;
    mutex & operator = (const mutex & ) = delete;
    ~mutex()
    {
    }

    inline void		lock(){
      thread_local static thrd_id_t thrd_id = rt::get_thrd_id();
      m_thread_conflicts[thrd_id] = m_locked.load();
      callback_prelock(this);
      m_mutex.lock();
      m_locked.store(true);
      callback_postlock(this);
    }
    void		unlock(){
      m_mutex.unlock();
      static double alpha = 0.5;
      static double beta = 1-alpha;
      static auto  cmp_func = [](double x, double n){return alpha *x + beta * n;};
      m_locked.store(false);
      thread_schedule_cost() = is_thread_conflict() ? 
                               cmp_func(thread_schedule_cost(), 1) :
                               cmp_func(thread_schedule_cost(), 0);
      callback_postunlock(this);
    }

    bool                is_thread_conflict() const{
      thread_local static thrd_id_t thrd_id = rt::get_thrd_id();
      return m_thread_conflicts[thrd_id];
    }
    double & thread_schedule_cost() {
      thread_local static thrd_id_t thrd_id = rt::get_thrd_id();
      return m_thread_schedule_cost[thrd_id];
    }

    inline mutex_id_t	id() {return this;}
public:
    //several hook function
    std::function<void (mutex_id_t)> callback_prelock;
    std::function<void (mutex_id_t)> callback_postlock;
    std::function<void (mutex_id_t)> callback_postunlock;
protected:
    std::mutex  m_mutex;
    std::atomic_bool  m_locked;
    std::vector<bool> m_thread_conflicts;
    std::vector<double> m_thread_schedule_cost;
};//end class mutex
}//end namespace ff

#endif
