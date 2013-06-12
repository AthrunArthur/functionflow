#ifndef FF_RUNTIME_RUNTIME_H_
#define FF_RUNTIME_RUNTIME_H_
#include <memory>
#include "runtime/rtcmn.h"
#include "runtime/task_queue.h"
#include "runtime/threadpool.h"
#include <thread>

namespace ff {
namespace rt
{

class threadpool;
class runtime;
typedef std::shared_ptr<runtime> runtime_ptr;
class runtime
{
protected:
    runtime();
    runtime(const runtime &) = delete;

public:
    static runtime_ptr	instance();

    //each thread run
    void			thread_run(const std::thread::id & id);
protected:
    void			init();
protected:
    std::unique_ptr<task_queue>		m_oReadyTasks;
    std::unique_ptr<task_queue>		m_oWaitTasks;
    std::unique_ptr<threadpool>		m_oTP;
    static runtime_ptr s_pInstance;
};//end class runtime
}//end namespace rt
}//end namespace ff
#endif