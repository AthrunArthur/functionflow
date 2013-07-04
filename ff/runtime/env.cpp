#include "runtime/env.h"
#include "runtime/rtcmn.h"
#include "runtime/runtime.h"
#include "common/log.h"
#include <thread>

namespace ff {
namespace rt {

runtime_ptr runtime::s_pInstance(nullptr);

runtime::runtime()
    : m_pReadyTasks(new task_queue())
    , m_pTP(new threadpool())
    , m_bAllThreadsQuit(false)
{
    LOG_INFO(rt)<<"runtime::runtime()";
}
runtime::~runtime()
{
    LOG_INFO(rt)<<"runtime::~runtime() waiting...";
    m_bAllThreadsQuit.store(true);
    m_pTP->join();
    LOG_INFO(rt)<<"runtime::~runtime(), exit!";
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
    LOG_INFO(thread)<<"runtime::thread_run, start thread, id:"<<id;
    auto info = RTThreadInfo::instance();
	info->clear_main_thread();
    setjmp(info->get_entry_point().get());

    while(!m_pReadyTasks->empty() &&
            !m_bAllThreadsQuit.load())
    {
        info->check_and_run_paused_ctx();
        if(m_pReadyTasks->empty())
            std::this_thread::yield();
        else
            take_one_task_and_run(id);
    }
}

void runtime::init()
{
    int thrd_num = std::thread::hardware_concurrency();
    LOG_INFO(thread)<<"runtime::init, thread num:"<<thrd_num;
    m_pTP->run(thrd_num, [this](std::thread::id & id) {
        thread_run(id);
    });
    LOG_INFO(thread)<<"runtime::init over"<<thrd_num;
}

bool runtime::take_one_task_and_run(const std::thread::id & id)
{
    task_queue * tq = runtime::instance()->getReadyTasks();
    task_base_ptr pTask;
    bool b = tq->pop(pTask);
    if(b)
    {
        LOG_INFO(thread)<<"runtime::take_one_task_and_run, got task "<<pTask.get();
        pTask->run();
    }
    return b;
}

void	schedule(task_base_ptr p)
{
    auto r = runtime::instance();
    r->getReadyTasks()->push_back(p);
	LOG_INFO(rt)<<"schedule, task "<<p.get()<<" has been push_back!";
}

void yield()
{
	LOG_INFO(rt)<<"yield(), enter!";
    auto info = RTThreadInfo::instance();
    ff::jmp_buf_ptr ctx = make_shared_jmp_buf();
    if(setjmp(ctx.get()) == 0)
    {
        LOG_INFO(rt)<<"yield, save ctx and jmp...";
	ctx_pdict cp;
  cp.ctx = ctx;
  cp.pdict = [](){return true;};
        info->get_to_exe_ctxs().push_back(cp);
        longjmp(info->get_entry_point().get(), 1);
    }
    else {
        info->erase_runned_ctx(ctx);
        LOG_INFO(rt)<<"yield, jmped into ctx...";
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
        auto f = pair.pdict;
        if(f())
        {
            longjmp(pair.ctx.get(), 1);
        }
    }
}//end check_and_run_paused_ctx

void RTThreadInfo::erase_runned_ctx(::ff::jmp_buf_ptr ctx)
{
  ctx_pdict cp;
  cp.ctx = ctx;
  cp.pdict = [](){return true;};
    m_oToExeCtxs.push_back(cp);
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
