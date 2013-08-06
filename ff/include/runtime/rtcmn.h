#ifndef FF_RUNTIME_RTCMN_H_
#define FF_RUNTIME_RTCMN_H_
#include <thread>
#include <atomic>
#include <mutex>
#include "runtime.h"
//#include "common/log.h"


namespace ff {
namespace rt {
typedef int32_t thrd_id_t;

inline size_t  hardware_concurrency(){
	static int t = std::thread::hardware_concurrency();
	return t;
}
inline size_t rt_concurrency()
{
	return hardware_concurrency() + 1;
}

thrd_id_t get_thrd_id();

void	schedule(task_base_ptr p);

//Give other tasks opportunities to run!
void yield();

template <class Func>
void 	yield_and_ret_until(Func f)
{
//	LOG_INFO(rt)<<"yield_and_ret_until(), enter...";
	while(!f())
	{
		runtime_ptr r = runtime::instance();
			
		if(r->take_one_task_and_run())
		{
			//LOG_INFO(rt)<<"yield_and_ret_until(), recursively run...";
		}
		else{
			yield();
		}
	}
}

}//end namespace rt
}//end namespace ff
#endif
