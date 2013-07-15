#ifndef FF_RUNTIME_RTCMN_H_
#define FF_RUNTIME_RTCMN_H_
#include <thread>
#include <atomic>
#include <mutex>
#include "runtime.h"
#include "common/log.h"


namespace ff {
namespace rt {

void	schedule(task_base_ptr p){
	runtime_ptr r = runtime::instance();
	r->schedule(p);
};

//Give other tasks opportunities to run!
void yield(){
	std::this_thread::yield();
};

template <class Func>
void 	yield_and_ret_until(Func f)
{
	LOG_INFO(rt)<<"yield_and_ret_until(), enter...";
	while(!f())
	{
		runtime_ptr r = runtime::instance();
			
		if(r->take_one_task_and_run())
		{
			LOG_INFO(rt)<<"yield_and_ret_until(), recursively run...";
		}
		else{
			std::this_thread::yield();
		}
	}
}

}//end namespace rt
}//end namespace ff
#endif
