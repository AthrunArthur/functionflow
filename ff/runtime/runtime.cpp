/***********************************************
The MIT License (MIT)

Copyright (c) 2012 Athrun Arthur <athrunarthur@gmail.com>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*************************************************/
#include "runtime/rtcmn.h"
#include "runtime/runtime.h"
#include "utilities/mutex.h"
#ifdef FUNCTION_FLOW_DEBUG
#include "runtime/record.h"
#endif
#ifdef COUNT_TIME
#include "utilities/timer.h"
#endif

#ifdef FUNC_INVOKE_COUNTER
#include "utilities/func_invoke_counter.h"
#endif

#ifdef FUNCTION_FLOW_DEBUG
#include <signal.h>
void sighandler(int signum)
{
  std::cout<<"Caught signal:" <<signum<<std::endl;
  std::cout<<"Now dumping info..."<<std::endl;
  ff::rt::all_records::getInstance()->dump_all();
  exit(-1);
}
#endif

namespace ff {

namespace rt {
#ifdef FUNCTION_FLOW_DEBUG
 all_records * all_records::s_pInstance = nullptr;
#endif
std::shared_ptr<runtime_deletor> runtime_deletor::s_pInstance(nullptr);
runtime_ptr runtime::s_pInstance(nullptr);
std::once_flag		runtime::s_oOnce;

void schedule(task_base_ptr p)
{   static runtime_ptr r = runtime::instance();
    r->schedule(p);
    _DEBUG(LOG_INFO(rt)<<"schedule end ")
}
#ifdef USING_MIMO_QUEUE
void schedule(task_base_ptr p, thrd_id_t target_thrd)
{
    static runtime_ptr r = runtime::instance();
    r->schedule(p, target_thrd);
}
#endif
void yield()
{
//	std::this_thread::sleep_for(std::chrono::nanoseconds(1));
    std::this_thread::yield();
}


runtime::runtime()
    : m_pTP(new threadpool())
    , m_oQueues()
    , m_bAllThreadsQuit(false) {};

runtime::~runtime()
{
    m_bAllThreadsQuit = true;
#ifdef FUNCTION_FLOW_DEBUG
    std::cout<<"~runtime(), start quiting runtime..."<<std::endl;
#endif
    m_pTP->join();
#ifdef FUNCTION_FLOW_DEBUG
    std::cout<<"~runtime(), all threads quit!"<<std::endl;
#endif
}

runtime_ptr 	runtime::instance()
{
    if(!s_pInstance)
#ifdef  CLANG_LLVM
        init();
#else
        std::call_once(s_oOnce, runtime::init);
#endif
    return s_pInstance;
}

void			runtime::init()
{
    s_pInstance = new runtime();
    runtime_deletor::s_pInstance = std::make_shared<runtime_deletor>(s_pInstance);
    auto thrd_num = hardware_concurrency();
    for(int i = 0; i<rt_concurrency(); ++i)
    {
        s_pInstance->m_oQueues.push_back(std::unique_ptr<work_stealing_queue>(new work_stealing_queue()));
        s_pInstance->m_oExeOverTasks.push_back(0);
        s_pInstance->m_oScheduleTasks.push_back(0);
    }
#ifdef FUNCTION_FLOW_DEBUG
    all_records * prs = all_records::getInstance();
    prs->init(thrd_num, "wsr.dat");
    struct sigaction act_h;
    struct sigaction old_act;
    act_h.sa_handler = sighandler;
    memset(&act_h, 0, sizeof(act_h));
    act_h.sa_handler = sighandler;

    sigaction(SIGINT,&act_h,&old_act);
    sigaction(SIGSEGV, &act_h, &old_act);
#endif

#ifdef FUNC_INVOKE_COUNTER
    ::ff::func_invoke_counter::init(thrd_num + 1);
#endif
    _DEBUG(LOG_INFO(rt)<<"init thread num:"<<thrd_num)
    set_local_thrd_id(0);

    for(int i = 1; i< thrd_num + 1; ++i)
    {
        s_pInstance->m_pTP->run([i]() {
            auto r = runtime::instance();
            set_local_thrd_id(i);
            r->thread_run();
        });
    }
    _DEBUG(LOG_INFO(rt)<<"runtime::init over"<<thrd_num)
}

void	runtime::schedule(task_base_ptr p)
{
    thread_local static int i = get_thrd_id();
    m_oScheduleTasks[i] ++;
    _DEBUG(LOG_INFO(rt)<<"runtime::schedule() id:"<<i<<" task: "<<p.get();)
#ifdef USING_FLEXIBLE_QUEUE
    m_oQueues[i] ->push_back(p);
    CTE(timer::para_timer);
#else
    if(!m_oQueues[i] ->push_back(p))
    {
      CTE(timer::para_timer);
      run_task(p);
    }
    else
    {
      CTE(timer::para_timer);
    }
#endif
    _DEBUG(LOG_INFO(rt)<<"runtime::schedule() end id:"<<i<<" task: "<<p.get();)
}


#ifdef USING_MIMO_QUEUE
void runtime::schedule(task_base_ptr p, thrd_id_t target_thrd)
{
    thread_local static int i = get_thrd_id();
    //target_thrd = get_idle();
    if(i == target_thrd || ! m_oQueues[target_thrd]->concurrent_push(p))
        m_oQueues[target_thrd]->push_back(p);
}
#endif
bool		runtime::take_one_task(task_base_ptr & pTask)
{
    bool b = false;
    CT(timer::schedule_timer);
    FIC(runtime_take_one_task)

#ifdef FUNC_INVOKE_COUNTER
    thread_local static uint64_t tc_counter = 0;
    tc_counter ++;
    if(tc_counter % (1<<31) == 0){
        std::cout<<"\n\n*****func invocation counter:\n";
        std::cout<<::ff::func_invoke_counter::status()<<std::endl;
    }
#endif

    thread_local static int i = get_thrd_id();
    b = m_oQueues[i]->pop(pTask);
    if(!b)
    {
        b = steal_one_task(pTask);
        _DEBUG(
        if(b)
        {
                if(pTask == nullptr){
                    ff::rt::all_records::getInstance()->dump_all();
                    std::cout<<"steal invalid task"<<std::endl;
                }
                assert(pTask !=nullptr && "Steal invalid task!");
        })
    }

    else{
#ifdef FUNCTION_FLOW_DEBUG
      if(pTask==nullptr)
      {
        ff::rt::all_records::getInstance()->dump_all();
        std::cout<<"pop invalid task"<<std::endl;
        assert(pTask != nullptr && "Invalid task");
      }
#endif
    }
    CTE(timer::schedule_timer);
    return b;
}

void 			runtime::run_task(task_base_ptr & pTask)
{
  FIC(runtime_run_task)
#ifdef COUNT_TIME
  auto s = single_timer::st_clock_t::now();
#endif
  thread_local static int cur_id = get_thrd_id();
  pTask->run();
  m_oExeOverTasks[cur_id] ++;
#ifdef COUNT_TIME
  auto e = single_timer::st_clock_t::now();
  timer_instance().append<timer::runner_timer>(e-s);
#endif
}
#if 0
void 			runtime::run_task(task_base_ptr & pTask)
{
    TLS_t int i = get_thrd_id();
    int take_times = 0;
    int steal_times = 0;
    double least_cost = 1;
    constexpr int least_times = 5;
START:

    _DEBUG(LOG_INFO(rt)<<"run_task() id:"<<get_thrd_id()<<" get task... "<<pTask.get();)
    if(pTask->getHoldMutex() != invalid_mutex_id)
    {
        mutex * pmutex = static_cast<mutex *>(pTask->getHoldMutex());
        double hist_cost = pmutex->thread_schedule_cost();
        m_oHPMutex.get_hazard_pointer().store(pTask->getHoldMutex());
        if(m_oHPMutex.outstanding_hazard_pointer_for(pTask->getHoldMutex()))
        {
            if(take_times < least_times || hist_cost > least_cost)
            {
                m_oHPMutex.get_hazard_pointer().store(invalid_mutex_id);
                m_oQueues[i]->push_back(pTask);
                if(take_times &0x1F == 0 )
                {
		    steal_times ++;
                    if(!steal_one_task(pTask))
		    {
		      take_one_task(pTask);
		    }
                }
                else
		{
		  take_one_task(pTask);
		}
		take_times ++;
                least_cost = least_cost > hist_cost ?
                  hist_cost : least_cost;
                goto START;
            }
            else
            {
                std::atomic<void *> & t = m_oHPMutex.get_hazard_pointer();
                pmutex->callback_postunlock = [ & t](mutex_id_t no_use){
                  t.store(invalid_mutex_id);
                };
                pTask->run();
            }
        }
        else
        {
            std::atomic<void *> & t = m_oHPMutex.get_hazard_pointer();
            pmutex->callback_postunlock = [ & t](mutex_id_t no_use){
              t.store(invalid_mutex_id);
            };
            pTask->run();
            //m_oHPMutex.get_hazard_pointer().store(invalid_mutex_id);
        }
    }
    else
        pTask->run();
}
#endif
#if 0 //The old scheduler
void 			runtime::run_task(task_base_ptr & pTask)
{
    TLS_t int i = get_thrd_id();
    int take_times = 0;
    int steal_times = 0;
START:

    _DEBUG(LOG_INFO(rt)<<"run_task() id:"<<get_thrd_id()<<" get task... "<<pTask.get();)
    if(pTask->getHoldMutex() != invalid_mutex_id)
    {
        m_oHPMutex.get_hazard_pointer().store(pTask->getHoldMutex());
        if(m_oHPMutex.outstanding_hazard_pointer_for(pTask->getHoldMutex()))
        {
            if(take_times >= m_oQueues[i]->size()+1 && steal_times > 2*rt::rt_concurrency())
            {
                pTask->run();
                m_oHPMutex.get_hazard_pointer().store(invalid_mutex_id);
            }
            else {
                m_oHPMutex.get_hazard_pointer().store(invalid_mutex_id);
                m_oQueues[i]->push_back(pTask);
                if(take_times & 0x1 )
                {
		    steal_times ++;
                    if(!steal_one_task(pTask))
		    {
		      take_one_task(pTask);
		    }
		    take_times ++;
                }
                else
		{
		  take_one_task(pTask);
		  take_times ++;
		}
                goto START;
            }

        }
        else
        {
            pTask->run();
            m_oHPMutex.get_hazard_pointer().store(invalid_mutex_id);
        }
    }
    else
        pTask->run();
}
#endif
void			runtime::thread_run()
{
    bool flag = false;
    thread_local static int cur_id = get_thrd_id();
    _DEBUG(LOG_INFO(rt)<<"runtime::thread_run() id:"<<cur_id<<" enter...")
    task_base_ptr pTask;
    CT(timer::total_timer);
    while(!m_bAllThreadsQuit)
    {
        _DEBUG(LOG_TRACE(rt)<<"thread run loop, thrd_id:"<<cur_id)
        flag = take_one_task(pTask);
        if(flag){
          run_task(pTask);
        }
        if(!flag)
            yield();
    }
    CTE(timer::total_timer);
}

bool		runtime::steal_one_task(task_base_ptr & pTask)
{
    thread_local static int cur_id = get_thrd_id();
    size_t dis = 1;
    size_t ts = m_oQueues.size();
    while((cur_id + dis)%ts !=cur_id)
    {
        _DEBUG(LOG_TRACE(rt)<<"try to steal task, thrd_id: "<<cur_id<<", victim id:"<<((cur_id + dis)%ts))
        if(m_oQueues[(cur_id + dis)%ts]->steal(pTask))
        {
            _DEBUG(LOG_INFO(rt)<<"thread id:"<<cur_id<<"stole a task "<< pTask.get());
            return true;
        }
        dis ++;
    }
    return false;
}

bool		runtime::is_idle()
{
    thread_local static int cur_id = get_thrd_id();
    size_t ts = m_oQueues.size();
    if(m_oQueues[cur_id]->size() != 0)
        return false;

    size_t dis = 1;
    while((cur_id + dis)%ts !=cur_id)
    {
        if(m_oQueues[(cur_id + dis)%ts]->size() != 0)
            return false;
        dis ++;
    }
    return true;
}

thrd_id_t	runtime::get_idle()
{
    thread_local static  int cur_id = get_thrd_id();
    size_t ts = m_oQueues.size();
    size_t dis = 1;
    thrd_id_t idle_id = (cur_id + 1) %ts;
    auto idle_queue_size = m_oQueues[(cur_id + 1)%ts]->size();
    while((cur_id + dis) %ts != cur_id)
    {
        thrd_id_t id = static_cast<thrd_id_t>((cur_id + dis)%ts);
        auto t = m_oQueues[id]->size();

        dis ++;
        if(t == 0)
            return id;
        if( t < idle_queue_size )
        {
            idle_id = id;
            idle_queue_size = t;
        }
    }
    return idle_id;
}

std::tuple<uint64_t, uint64_t> runtime::current_task_counter()
{
  uint64_t r0=0, r1=0;
  for(uint64_t t : m_oExeOverTasks)
    r0 += t;
  for(uint64_t t : m_oScheduleTasks)
    r1 += t;
  return std::make_tuple(r0, r1);
}

}//end namespace rt
bool is_idle()
{
    static rt::runtime_ptr r = rt::runtime::instance();
    return r->is_idle();
}
}//end namespace ff
