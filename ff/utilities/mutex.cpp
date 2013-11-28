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
#include "utilities/mutex.h"
#include "utilities/mutex_manager.h"
#include "runtime/rtcmn.h"
#include "runtime/env.h"
namespace ff {
mutex::mutex()   /* : flag(ATOMIC_FLAG_INIT) */{
    for(int i = 0; i < rt::rt_concurrency(); i++)
      m_who_runs_me.push_back(0);
}

void mutex::lock() {
    //while(flag.test_and_set(std::memory_order_acquire)) ff::rt::yield();
  //ff::rt::yield_and_ret_until([this](){return m_mutex.try_lock();});
  m_mutex.lock();  
  m_who_runs_me[rt::get_thrd_id()] ++;
}

void mutex::unlock()
{   
  //flag.clear(std::memory_order_release);
  m_mutex.unlock();
}

thrd_id_t mutex::who_runs_most()
{
  int  m = 0;
  int id = 0;
  for(int i = 0; i < m_who_runs_me.size(); ++i)
    if(m_who_runs_me[i] > m)
    {
      m = m_who_runs_me[i];
      id = i;
    }
  return static_cast<thrd_id_t>(id);
}
namespace internal {
  std::shared_ptr<mutex_manager> mutex_manager::s_pInstance;
  
  mutex_manager::mutex_manager(){}
  mutex_manager & mutex_manager::instance()
  {
     if(!s_pInstance)
     {  s_pInstance = std::shared_ptr<mutex_manager>(new mutex_manager());
     for(int i = 0; i < rt::rt_concurrency(); ++i)
      s_pInstance->m_oMutexs.push_back(invalid_mutex_id);
     }
     return *s_pInstance;
  }
  
  void 		mutex_manager::hold_mutex(const mutex_id_t & id)
  {
    thread_local static thrd_id_t tid = rt::get_thrd_id();
    m_oMutexs[tid] = id;
  }
  void		mutex_manager::reset_mutex()
  {
    thread_local static thrd_id_t tid = rt::get_thrd_id();
    m_oMutexs[tid] = invalid_mutex_id;
  }
  
  thrd_id_t	mutex_manager::who_hold_mutex(const mutex_id_t & id)
  {
    for(size_t i = 0; i < m_oMutexs.size(); ++i)
    {
      if(m_oMutexs[i] == id)
	return static_cast<thrd_id_t>(i);
    }
    return invalid_thrd_id;
  }
}//end namespace internal
}//end namespace ff