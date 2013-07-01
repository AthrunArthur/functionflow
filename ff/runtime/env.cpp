#include "runtime/env.h"
#include "runtime/rtcmn.h"
#include "runtime/runtime.h"

#include <thread>

namespace ff {
namespace rt {
void	schedule(task_base_ptr p)
{
    auto r = runtime::instance();
    r->getReadyTasks()->push_back(p);
}

void yield()
{
    auto info = RTThreadInfo::instance();
    auto ctx = make_shared_jmp_buf();
    if(setjmp(ctx.get()) == 0)
    {
        info->get_to_exe_ctxs->push_back(std::make_tuple(ctx, []() {
            return true;
        });
                                         longjmp(info->get_entry_point().get(), 1);
    }
    else {
        info->erase_runned_ctx(ctx);
    }
}

std::shared_ptr<RTThreadInfo> RTThreadInfo::s_pInstance(nullptr);

std::shared_ptr<RTThreadInfo> RTThreadInfo::instance()
{
    if(!s_pInstance)
        s_pInstance = std::make_shared<RTThreadInfo>();
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
    m_oToExeCtxs.emplace_back(ctx);
}

threadpool::threadpool()
    : m_oThreads() {}

void threadpool::join()
{
for(auto t : m_oThreads)
    {
        t.join();
    }
}

}//end namespace rt
}//end namespace ff