#ifndef FF_RUNTIME_RUNTIME_H_
#define FF_RUNTIME_RUNTIME_H_
#include <memory>
#include "runtime/task_queue.h"
#include "runtime/threadpool.h"
#include <memory>
#include "common/log.h"

namespace ff {
namespace rt
{

class threadpool;
class runtime;
typedef runtime * runtime_ptr;


class runtime
{
protected:
    runtime();
    runtime(const runtime &) = delete;

public:
	
	virtual ~runtime();
    static runtime_ptr 	instance();

    void	schedule(task_base_ptr p);

    bool		take_one_task_and_run();

protected:
    //each thread run
    void			thread_run(size_t index);


    bool		steal_one_task_and_run(size_t cur_id);

    static void			init();
    
protected:
    std::unique_ptr<task_queue> 		m_pGlobalTasks;
    std::unique_ptr<threadpool> 		m_pTP;
    std::vector<std::unique_ptr<work_stealing_queue> >	m_oQueues;
	thread_local static work_stealing_queue *				m_pLQueue;
    std::atomic< bool>  				m_bAllThreadsQuit;

    static runtime_ptr s_pInstance;
	static std::once_flag			s_oOnce;
};//end class runtime


class runtime_deletor
{
public:
    runtime_deletor(runtime *pRT): m_pRT(pRT) {};
    ~runtime_deletor() {
        delete m_pRT;
    };
    static std::shared_ptr<runtime_deletor> s_pInstance;
protected:
    runtime *	m_pRT;
};


}//end namespace rt
}//end namespace ff
#endif
