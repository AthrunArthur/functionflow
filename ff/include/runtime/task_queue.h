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
#ifndef FF_RUNTIME_TASK_QUEUE_H_
#define FF_RUNTIME_TASK_QUEUE_H_

#include "runtime/taskbase.h"
#ifdef USING_LOCK_FREE_QUEUE
#include "runtime/ring_buff.h"
#endif

#ifdef USING_MUTEX_QUEUE
#include "runtime/mutex_steal_queue.h"
#endif

#ifdef USING_FF_SPIN_STEAL_QUEUE
#include "runtime/spin_steal_queue.h"
#endif

#ifdef USING_MIMO_QUEUE
#include "runtime/mimo_queue.h"
#endif

#ifdef USING_WORK_STEALING_QUEUE
//#include "runtime/twsq.h"
#ifdef USING_FLEXIBLE_QUEUE
#include "runtime/gtwsq.h"
#else
#include "runtime/gtwsq_fixed.h"
#endif
#endif

#include "runtime/env.h"

namespace ff {
namespace rt {

#ifdef USING_MUTEX_QUEUE
typedef mutex_stealing_queue<task_base_ptr> work_stealing_queue;
#endif

#ifdef USING_LOCK_FREE_QUEUE
typedef nonblocking_stealing_queue<task_base_ptr, 6> work_stealing_queue;
#endif

#ifdef USING_FF_SPIN_STEAL_QUEUE
typedef spin_stealing_queue<task_base_ptr, 8> work_stealing_queue;
#endif

#ifdef USING_MIMO_QUEUE
typedef mimo_lock_free_queue<task_base_ptr, 6> work_stealing_queue;
#endif

#ifdef USING_WORK_STEALING_QUEUE
//typedef classical_work_stealing_queue<task_base_ptr, 20> work_stealing_queue;
typedef gcc_work_stealing_queue<task_base_ptr, 8> work_stealing_queue;
#endif

typedef work_stealing_queue * work_stealing_queue_ptr;
typedef std::vector<ctx_pdict_ptr> local_stack_queue;

}//end namespace rt
}//end namespace ff
#endif
