#ifndef FF_RUNTIME_RTCMN_H_
#define FF_RUNTIME_RTCMN_H_
#include <thread>
#include <atomic>
#include <mutex>
#include "runtime/env.h"
#include "runtime/taskbase.h"
#include "runtime.h"
#include "common/log.h"


namespace ff {
namespace rt {

void	schedule(task_base_ptr p);

//Give other tasks opportunities to run!
void yield();

//!This will save the context, and save it to the current RTThreadInfo.
template <class Func>
void 	yield_and_ret_until(Func f)
{
	LOG_INFO(rt)<<"yield_and_ret_until(), enter...";
	while(!f())
	{
		runtime_ptr r = runtime::instance();
		if(r->getReadyTasks()->empty())
			std::this_thread::yield();
		else
			runtime::take_one_task_and_run();
	}
}

/*
template <class Func>
void 	yield_and_ret_until(Func f)
{
    LOG_INFO(rt)<<"yield_and_ret_until(), enter...";
    auto info = RTThreadInfo::instance();

    if(info->is_main_thread())
    {
		LOG_INFO(rt)<<"yield_and_ret_until(), main thread...";
        if(setjmp(info->get_entry_point().get()) !=0)
		{
			if(f())
				return;
			std::this_thread::yield;
		}
    }

    auto ctx = make_shared_jmp_buf();
    if(setjmp(ctx.get()) == 0)
    {
        ctx_pdict cp;
        cp.ctx = ctx;
        cp.pdict = [f]() {
            return f();
        };
        info->get_to_exe_ctxs().push_back(cp);
        longjmp(info->get_entry_point().get(), 1);
    }
}*///end yield_and_ret_until

}//end namespace rt
}//end namespace ff
#endif
