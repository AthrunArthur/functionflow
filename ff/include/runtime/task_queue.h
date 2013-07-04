#ifndef FF_RUNTIME_TASK_QUEUE_H_
#define FF_RUNTIME_TASK_QUEUE_H_
#include "ff/blocking_queue.h"
#include "ff/nonblocking_queue.h"
#include "runtime/taskbase.h"
namespace ff {
namespace rt {
typedef ff::nonblocking_queue<task_base_ptr> task_queue;
}
}//end namespace ff
#endif