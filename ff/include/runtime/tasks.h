#ifndef FF_RUNTIME_TASKS_H_
#define FF_RUNTIME_TASKS_H_
#include "runtime/taskbase.h"

namespace ff{
	namespace rt {
		class user_task: task_base
		{
		};//end class user_task
		
		//This is for context
		class context_task:task_base
		{
		};//end class context_task;
		
		//This must be distributed to all worker thread
		//and the worker thread will quite when it receive this task
		class end_thread_task : task_base
		{
		};//end class end_thread_task
	}//end namespace ff
}//end namespace ff

#endif