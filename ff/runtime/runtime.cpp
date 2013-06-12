#include "runtime/runtime.h"
#include "runtime/env.h"
#include "runtime/tasks.h"

namespace ff {
namespace rt {
runtime_ptr runtime::s_pInstance = nullptr;
runtime::runtime()
: m_pReadyTasks()
, m_pWaitTasks()
, m_pTP(){}

runtime::~runtime()
{
	auto pEnv = environment::instance();
	int thrd_num = pEnv->get_thrd_num();
	for(int i = 0; i<thrd_num; ++i)
	{
		task_base_ptr pt = task_base_ptr(new end_thread_task());
		m_pReadyTasks->push_back(pt);
	}
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
	while(1)
	{
		task_base_ptr pTask;
		m_pReadyTasks->pop(pTask);
		if(pTask->getTK() == task_base::TKind::end_t)
			break;
		pTask->run(id);
	}
}

void runtime::init()
{
	auto pEnv = environment::instance();
	int thrd_num = pEnv->get_thrd_num();
	m_pTP->run(thrd_num, [this](std::thread::id & id){thread_run(id);});
}

}//end namespace rt
}//end namespace ff