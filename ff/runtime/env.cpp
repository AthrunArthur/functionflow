#include "runtime/env.h"
#include "runtime/rtcmn.h"
#include "runtime/runtime.h"

#include <thread>

namespace ff {
namespace rt {

runtime_ptr runtime::s_pInstance(nullptr);
runtime::runtime()
{
}
runtime_ptr runtime::instance()
{
	if(!s_pInstance)
	{
		s_pInstance = std::shared_ptr<runtime>(new runtime());
		s_pInstance->init();
	}
	return s_pInstance;
	
}
void runtime::thread_run(const std::thread::id & id)
{
  auto info = RTThreadInfo::instance();
  setjmp(info->get_entry_point().get());
  
  while(!m_pReadyTasks->empty() && 
    !m_bAllThreadsQuit.load())
  {
    if(m_pReadyTasks->empty())
      std::this_thread::yield();
    else
      take_one_task_and_run(id);
  }
}

void runtime::init()
{
  int thrd_num = std::thread::hardware_concurrency();
  m_pTP->run(thrd_num, [this](std::thread::id & id){thread_run(id);});
}

bool runtime::take_one_task_and_run(const std::thread::id & id)
{
  task_queue * tq = runtime::instance()->getReadyTasks();
  task_base_ptr pTask;
  bool b = tq->pop(pTask);
  if(b)
  {
    pTask->run();
 }
  return b;
}

void	schedule(task_base_ptr p)
{
    auto r = runtime::instance();
    r->getReadyTasks()->push_back(p);
}

void yield()
{
    auto info = RTThreadInfo::instance();
    ff::jmp_buf_ptr ctx = make_shared_jmp_buf();
    if(setjmp(ctx.get()) == 0)
    {
        info->get_to_exe_ctxs().push_back(std::make_tuple(ctx, []() {
            return true;
        }));
                                         longjmp(info->get_entry_point().get(), 1);
    }
    else {
        info->erase_runned_ctx(ctx);
    }
}

thread_local std::shared_ptr<RTThreadInfo> RTThreadInfo::s_pInstance(nullptr);

std::shared_ptr<RTThreadInfo> RTThreadInfo::instance()
{
    if(!s_pInstance)
        s_pInstance = std::shared_ptr<RTThreadInfo>(new RTThreadInfo());
    return s_pInstance;
}
void RTThreadInfo::check_and_run_paused_ctx()
{
for(auto pair : m_oToExeCtxs)
    {
        auto f = std::get<1>(pair);
        if(f())
        {
            longjmp(std::get<0>(pair).get(), 1);
        }
    }
}//end check_and_run_paused_ctx

void RTThreadInfo::erase_runned_ctx(::ff::jmp_buf_ptr ctx)
{
    m_oToExeCtxs.push_back(std::make_tuple(ctx, [](){return true;}));
}

threadpool::threadpool()
    : m_oThreads() {}

void threadpool::join()
{
for(auto t : m_oThreads)
    {
        t->join();
    }
}

}//end namespace rt
}//end namespace ff
