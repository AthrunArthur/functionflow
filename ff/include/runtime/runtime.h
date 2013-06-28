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
    virtual ~runtime();
    static runtime_ptr	instance();

    //each thread run
    void			thread_run(const std::thread::id & id);
	
	task_queue *	getReadyTasks() {return m_pReadyTasks.get();}
	task_queue *	getWaitTasks() {return m_pWaitTasks.get();}
	threadpool *	getThreadPool() {return m_pTP.get();}
	
protected:
    void			init();
	
	static bool		take_one_task_and_run(const std::thread::id & id);
protected:
    std::unique_ptr<task_queue>		m_pReadyTasks;
    std::unique_ptr<task_queue>		m_pWaitTasks;
    std::unique_ptr<threadpool>		m_pTP;
	std::atomic_bool				m_bAllThreadsQuit;
    static runtime_ptr s_pInstance;
};//end class runtime
}//end namespace rt
}//end namespace ff
#endif