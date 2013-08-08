#include "runtime/rtcmn.h"
#include "runtime/runtime.h"

namespace ff {
namespace rt {
  std::shared_ptr<runtime_deletor> runtime_deletor::s_pInstance(nullptr);
runtime_ptr runtime::s_pInstance(nullptr);
std::once_flag		runtime::s_oOnce;


void schedule(task_base_ptr p)
{   static runtime_ptr r = runtime::instance();
    r->schedule(p);
}

void yield()
{
//	std::this_thread::sleep_for(std::chrono::nanoseconds(1));
    std::this_thread::yield();
}


runtime::runtime()
    : m_pTP(new threadpool())
    , m_oQueues()
    , m_oLocalCtxs()
    , m_bAllThreadsQuit(false) {};

runtime::~runtime()
{
    m_bAllThreadsQuit = true;
    _DEBUG(LOG_INFO(thread)<<"runtime::~runtime(), start killing worker thread...";)
    m_pTP->join();

}

runtime_ptr 	runtime::instance()
{
    if(!s_pInstance)
        std::call_once(s_oOnce, runtime::init);
    return s_pInstance;
}

void			runtime::init()
{
    s_pInstance = new runtime();
    runtime_deletor::s_pInstance = std::make_shared<runtime_deletor>(s_pInstance);
    int thrd_num = hardware_concurrency();
    for(int i = 0; i<rt_concurrency(); ++i)
    {
        s_pInstance->m_oQueues.push_back(std::unique_ptr<work_stealing_queue>(new work_stealing_queue()));
	s_pInstance->m_oLocalCtxs.push_back(std::unique_ptr<local_stack_queue>(new local_stack_queue()));
    }
    //  LOG_INFO(thread)<<"runtime::init, thread num:"<<thrd_num;
    set_local_thrd_id(0);
    for(int i = 1; i< thrd_num + 1; ++i)
    {
        s_pInstance->m_pTP->run([i]() {
            auto r = runtime::instance();
            set_local_thrd_id(i);
            r->thread_run();
        });
    }
    //LOG_INFO(thread)<<"runtime::init over"<<thrd_num;
}


void	runtime::schedule(task_base_ptr p)
{
    _DEBUG(LOG_INFO(thread)<<"runtime::schedule() task: "<<p.get();)
    static int i = get_thrd_id();
    m_oQueues[i] ->push_back(p);
}

bool		runtime::take_one_task_and_run()
{
    task_base_ptr pTask;
    bool b = false;
    _DEBUG(LOG_INFO(thread)<<"take_one_task_and_run() try to fetch task... ";)
    static int i = get_thrd_id();
    b = m_oQueues[i]->pop(pTask);
    if(b)
    {
        pTask->run();
    }
    else{
      b = steal_one_task_and_run();
    }
    return b;
}

void			runtime::thread_run()
{
    //LOG_INFO(thread)<<"runtime::thread_run() id:"<<index;
    bool flag = false;
    while(!m_bAllThreadsQuit)
    {
        flag = take_one_task_and_run();
        //if(!flag)
        //    flag = steal_one_task_and_run();
        if(!flag)
            yield();
    }
}

bool		runtime::steal_one_task_and_run()
{
  static int cur_id = get_thrd_id();
    size_t dis = 1;
    bool b = false;
    size_t ts = m_oQueues.size();
    task_base_ptr pTask;
    while((cur_id + dis)%ts !=cur_id)
    {
        if(m_oQueues[(cur_id + dis)%ts]->steal(pTask))
        {
            pTask->run();
            b = true;
            break;
        }
        dis ++;
    }
    return b;
}

bool runtime::restore_stack_and_run()
{
  static int cur_id = get_thrd_id();
  static local_stack_queue * p = m_oLocalCtxs[cur_id].get();
  CHECK_CTXS:
  for(local_stack_queue::iterator i = p->begin(); i != p->end(); i++)
  {
    ctx_pdict_ptr tp = * i;
    if(tp->pdict())
    {
      p->erase(i);
      longjmp(tp->ctx.get(), 1);
    }
  }
}

#if 0
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
            }
        }
        else
        {
            //std::cout<<"have to get a task from global..size:"<<m_pGlobalTasks->size()<<std::endl;
            if(m_pGlobalTasks->pop(pTask))
            {
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
#endif
}//end namespace rt
}//end namespace ff
