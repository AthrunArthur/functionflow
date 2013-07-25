#ifndef FF_RUNTIME_RUNTIME_H_
#define FF_RUNTIME_RUNTIME_H_
#include <memory>
#include "runtime/task_queue.h"
#include "runtime/threadpool.h"
#include <memory>
//#include <boost/concept_check.hpp>
//#include "common/log.h"

namespace ff {
namespace rt
{

class threadpool;
class runtime;
typedef runtime * runtime_ptr;


class runtime
{
protected:
    runtime()
        : m_pGlobalTasks(new task_queue())
        , m_pTP(new threadpool())
        , m_oQueues()
        , m_bAllThreadsQuit(false) {};
    runtime(const runtime &) = delete;

public:
	
	virtual ~runtime()
    {
		m_bAllThreadsQuit = true;
        m_pTP->join();
    }
    static runtime_ptr 	instance();

    void	schedule(task_base_ptr p)
	{
	//	LOG_INFO(thread)<<"runtime::schedule() task: "<<p.get();
		if(m_pLQueue != nullptr)
		{
			std::cout<<"schedule to local task"<<std::endl;
			m_pLQueue->push_back(p);
		}
		else 
		{
			std::cout<<"schedule to global queue"<<std::endl;
			m_pGlobalTasks->push_back(p);
		}
		
	}

    bool		take_one_task_and_run()
    {
        task_base_ptr pTask;
        bool b = false;
		//LOG_INFO(thread)<<"take_one_task_and_run() try to fetch task... ";
        if(m_pLQueue != nullptr)
            b = m_pLQueue->pop(pTask);
        else b = m_pGlobalTasks->pop(pTask);
        //LOG_INFO(thread)<<"take_one_task_and_run() fetch task ? "<<b;
        if(b)
        {
            //LOG_INFO(thread)<<"runtime::take_one_task_and_run, got task "<<pTask.get();
            pTask->run();
        }
        return b;
    }

protected:
    //each thread run
    void			thread_run(size_t index)
    {
		//LOG_INFO(thread)<<"runtime::thread_run() id:"<<index;
        bool flag = false;
        while(!m_bAllThreadsQuit)
        {
            flag = take_one_task_and_run();
            if(!flag)
                flag = steal_one_task_and_run(index);
            if(!flag)
                std::this_thread::yield();
        }
    }



    bool		steal_one_task_and_run(size_t cur_id)
    {
        size_t dis = 1;
        bool b = false;
        size_t ts = m_oQueues.size();
        task_base_ptr pTask;
       // while((cur_id + dis)%ts !=cur_id)
       // {
            if(m_oQueues[(cur_id + dis)%ts]->steal(pTask))
            {
				std::cout<<"got a task from neighbour"<<std::endl;
                pTask->run();
                b = true;
          //      break;
            }
            else //steal work from m_pGlobalTasks
			{
				task_block_ptr tbp;
				if(m_pGlobalTasks->steal_block(tbp))
				{
					std::cout<<"got tasks! from global"<<std::endl;
					m_pLQueue->internal_tasks().copy_from(tbp.get());
					if(m_pLQueue->pop(pTask))
					{
						pTask->run();
						b = true;
					}
				}
				else
				{
					//std::cout<<"have to get a task from global..size:"<<m_pGlobalTasks->size()<<std::endl;
					if(m_pGlobalTasks->pop(pTask))
					{
						std::cout<<"\t yes, done!"<<std::endl;
						pTask->run();
						b = true;
					}
					//if(b == false)
						//std::cout<<"\t but failed.."<<std::endl;
				}
			}
         //   dis ++;
        //}
        return b;
    }

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
