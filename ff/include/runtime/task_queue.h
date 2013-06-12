#ifndef FF_RUNTIME_TASK_QUEUE_H_
#define FF_RUNTIME_TASK_QUEUE_H_
#include "common/blocking_queue.h"
#include "runtime/taskbase.h"
namespace ff {
namespace rt {
typedef ff::utils::blocking_queue<task_base_ptr> task_queue;
}
}//end namespace ff
#endif