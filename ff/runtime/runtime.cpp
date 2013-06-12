#include "runtime/runtime.h"

namespace ff {
namespace rt {
	runtime_ptr runtime::s_pInstance = nullptr;
runtime::runtime()
: m_oReadyTasks()
, m_oWaitTasks()
, m_oTP(){}

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
	while(task_base_ptr pTask = m_oReadyTasks->pop())
	{
		if(pTask->getTK() == task_base::TKind::end_t)
			return;
		pTask->run();
	}
}

void runtime::init()
{
	
}

}
}