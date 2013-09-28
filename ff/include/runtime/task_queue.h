#ifndef FF_RUNTIME_TASK_QUEUE_H_
#define FF_RUNTIME_TASK_QUEUE_H_

#include "runtime/taskbase.h"
#ifdef USING_LOCK_FREE_QUEUE
#include "runtime/ring_buff.h"
#endif

#ifdef USING_FF_NONBLOCKING_QUEUE
#include "runtime/mutex_steal_queue.h"
#endif

#ifdef USING_FF_SPIN_STEAL_QUEUE
#include "runtime/spin_steal_queue.h"
#endif
#include "runtime/env.h"

namespace ff {
namespace rt {

#ifdef USING_FF_NONBLOCKING_QUEUE
typedef mutex_stealing_queue<task_base_ptr> work_stealing_queue;
#endif

#ifdef USING_LOCK_FREE_QUEUE
typedef nonblocking_stealing_queue<task_base_ptr, 8> work_stealing_queue;
#endif

#ifdef USING_FF_SPIN_STEAL_QUEUE
typedef spin_stealing_queue<task_base_ptr, 8> work_stealing_queue;
#endif

typedef work_stealing_queue * work_stealing_queue_ptr;
typedef std::vector<ctx_pdict_ptr> local_stack_queue;

}//end namespace rt
}//end namespace ff
#endif