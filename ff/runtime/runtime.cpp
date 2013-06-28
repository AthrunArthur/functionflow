#include "runtime/runtime.h"
#include "runtime/env.h"
#include "runtime/tasks.h"

namespace ff {
namespace rt {
runtime_ptr runtime::s_pInstance = nullptr;
runtime::runtime()
: m_pReadyTasks()
, m_pWaitTasks()
, m_pTP()
, m_bAllThreadsQuit(false){}

runtime::~runtime()
{
	m_bAllThreadsQuit.store(true);
	m_pTP->join();
}

runtime_ptr runtime::instance()
{
	if(s_pInstance == nullptr)
	{
		s_pInstance = runtime_ptr(new runtime());
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
	auto pEnv = environment::instance();
	int thrd_num = pEnv->get_thrd_num();
	m_pTP->run(thrd_num, [this](std::thread::id & id){thread_run(id);});
}

bool runtime::take_one_task_and_run(const std::thread::id & id)
{
	task_queue * tq = runtime::instance()->getReadyTasks();
	task_base_ptr pTask;
	bool b = tq->pop(pTask);
	if(b)
	{
		pTask->run(id);
	}
	return b;
}
}//end namespace rt
}//end namespace ff