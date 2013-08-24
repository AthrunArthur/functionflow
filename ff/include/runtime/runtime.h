#ifndef FF_RUNTIME_RUNTIME_H_
#define FF_RUNTIME_RUNTIME_H_
#include <memory>
#include "runtime/task_queue.h"
#include "runtime/threadpool.h"
#include <memory>
#include "runtime/env.h"
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
    void			thread_run();


    bool		steal_one_task_and_run();
    
    static void			init();

protected:
    std::unique_ptr<threadpool> 		m_pTP;
    std::vector<std::unique_ptr<work_stealing_queue> >	m_oQueues;
    
//    thread_local static work_stealing_queue *				m_pLQueue;
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

void	schedule(task_base_ptr p);

template <class Func>
void 	yield_and_ret_until(Func f)
{
//	LOG_INFO(rt)<<"yield_and_ret_until(), enter...";
    thread_local static int cur_id = get_thrd_id();
    thread_local static runtime_ptr r = runtime::instance();
    while(!f())
    {

        if(r->take_one_task_and_run())
        {
            //LOG_INFO(rt)<<"yield_and_ret_until(), recursively run...";
        }
        else {
            yield();
        }
    }
}


}//end namespace rt
}//end namespace ff
#endif
