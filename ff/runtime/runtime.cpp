#include "runtime/rtcmn.h"
#include "runtime/runtime.h"

namespace ff{
namespace rt{

void schedule(task_base_ptr p)
{	runtime_ptr r = runtime::instance();
	r->schedule(p);
}

void yield()
{
	std::this_thread::yield();
}

runtime::runtime()
        : m_pGlobalTasks(new task_queue())
        , m_pTP(new threadpool())
        , m_oQueues()
        , m_bAllThreadsQuit(false) {};
		
runtime::~runtime()
    {
		m_bAllThreadsQuit = true;
        m_pTP->join();
    }		
	
	    void	runtime::schedule(task_base_ptr p)
	{
		_DEBUG(LOG_INFO(thread)<<"runtime::schedule() task: "<<p.get();)
		if(m_pLQueue != nullptr)
			m_pLQueue->push_back(p);
		else
			m_pGlobalTasks->push_back(p);
	}

    bool		runtime::take_one_task_and_run()
    {
        task_base_ptr pTask;
        bool b = false;
		_DEBUG(LOG_INFO(thread)<<"take_one_task_and_run() try to fetch task... ";)
        if(m_pLQueue != nullptr)
            b = m_pLQueue->pop(pTask);
        else b = m_pGlobalTasks->pop(pTask);
        //LOG_INFO(thread)<<"take_one_task_and_run() fetch task ? "<<b;
        if(b)
        {
            //LOG_INFO(thread)<<"runtime::take_one_task_and_run, got task "<<pTask.get();
            pTask->run();
			if(pTask->isDelByRT())
				delete pTask;
        }
        return b;
    }
	
	void			runtime::thread_run(size_t index)
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



    bool		runtime::steal_one_task_and_run(size_t cur_id)
    {
        size_t dis = 1;
        bool b = false;
        size_t ts = m_oQueues.size();
        task_base_ptr pTask;
       // while((cur_id + dis)%ts !=cur_id)
       // {
            if(m_oQueues[(cur_id + dis)%ts]->steal(pTask))
            {
                pTask->run();
				if(pTask->isDelByRT())
					delete pTask;
                b = true;
          //      break;
            }
            else //steal work from m_pGlobalTasks
			{
				task_block_ptr tbp;
				if(m_pGlobalTasks->steal_block(tbp))
				{
					m_pLQueue->internal_tasks().copy_from(tbp.get());
					if(m_pLQueue->pop(pTask))
					{
						pTask->run();
						b = true;
						if(pTask->isDelByRT())
							delete pTask;
					}
				}
				else
				{
					//std::cout<<"have to get a task from global..size:"<<m_pGlobalTasks->size()<<std::endl;
					if(m_pGlobalTasks->pop(pTask))
					{
						pTask->run();
						if(pTask->isDelByRT())
							delete pTask;
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
	
}//end namespace rt
}//end namespace ff
