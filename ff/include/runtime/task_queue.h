#ifndef FF_RUNTIME_TASK_QUEUE_H_
#define FF_RUNTIME_TASK_QUEUE_H_
#include "common/blocking_queue.h"
#include "common/nonblocking_queue.h"
#include "runtime/taskbase.h"
namespace ff {
namespace rt {
typedef ff::utils::nonblocking_queue<task_base_ptr> task_queue;
}
}//end namespace ff
#endif