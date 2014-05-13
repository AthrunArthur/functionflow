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
#ifdef __GUNC__
#include <time.h>
#endif

namespace ff {
static int gmcounter = 0;

class mutex
{
public:
    mutex(): m_mutex()
    , m_iContentTimes(0)
    , m_iPeriods() {};
    
    mutex(const mutex &) = delete;
    mutex & operator = (const mutex & ) = delete;
    ~mutex()
    {
      std::stringstream ss;
      ss<<"mutex_" << gmcounter;
      gmcounter ++;
      std::ofstream f;
      f.open(ss.str());
      for(auto x : m_iPeriods)
      {
        f<<x<<'\n';
      }
      f.close();
    }

    inline void		lock(){
      struct timespec t;
      clock_gettime(CLOCK_MONOTONIC, &t);
      m_mutex.lock();
      struct timespec t2;
      clock_gettime(CLOCK_MONOTONIC, &t2);
      int64_t period = (t2.tv_sec - t.tv_sec) * 1000000000 + t2.tv_nsec - t.tv_nsec;
      m_iPeriods.push_back(period);
      m_iContentTimes.store(static_cast<uint64_t>( 0.7 * m_iContentTimes.load() + 0.3 * period));

    }
    void		unlock(){m_mutex.unlock();}

    inline mutex_id_t	id() {return this;}
    
protected:
    std::mutex  m_mutex;
    std::atomic_ullong m_iContentTimes;
    std::vector<int64_t> m_iPeriods;
};//end class mutex
}//end namespace ff

#endif
