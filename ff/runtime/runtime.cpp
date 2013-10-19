#include "runtime/rtcmn.h"
#include "runtime/runtime.h"

#include <pthread.h>

namespace ff {

namespace rt {
std::shared_ptr<runtime_deletor> runtime_deletor::s_pInstance(nullptr);
runtime_ptr runtime::s_pInstance(nullptr);
std::once_flag		runtime::s_oOnce;


void schedule(task_base_ptr p)
{   static runtime_ptr r = runtime::instance();
    r->schedule(p);
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
        std::call_once(s_oOnce, runtime::init);
    return s_pInstance;
}

void			runtime::init()
{
    s_pInstance = new runtime();
    runtime_deletor::s_pInstance = std::make_shared<runtime_deletor>(s_pInstance);
    int thrd_num = hardware_concurrency();
    for(int i = 0; i<rt_concurrency(); ++i)
    {
        s_pInstance->m_oQueues.push_back(std::unique_ptr<work_stealing_queue>(new work_stealing_queue()));
    }
    _DEBUG(LOG_INFO(rt)<<"init thread num:"<<thrd_num)
    set_local_thrd_id(0);

    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(0, &cpuset);
    auto s = pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpuset);

    for(int i = 1; i< thrd_num + 1; ++i)
    {
        s_pInstance->m_pTP->run([i]() {
            auto r = runtime::instance();
            set_local_thrd_id(i);
            cpu_set_t cpuset;
            CPU_ZERO(&cpuset);
            CPU_SET(i, &cpuset);
            auto s = pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpuset);
            _DEBUG(LOG_INFO(rt)<<"pthread_setaffinity_np ret: "<< s);
            r->thread_run();
        });
    }
    _DEBUG(LOG_INFO(rt)<<"runtime::init over"<<thrd_num)
}

void	runtime::schedule(task_base_ptr p)
{
    thread_local static int i = get_thrd_id();
    _DEBUG(LOG_INFO(rt)<<"runtime::schedule() id:"<<i<<" task: "<<p.get();)
    m_oQueues[i] ->push_back(p);
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
bool		runtime::take_one_task_and_run()
{
    task_base_ptr pTask;
    bool b = false;

    thread_local static int i = get_thrd_id();
    b = m_oQueues[i]->pop(pTask);
    if(b)
    {
        _DEBUG(LOG_INFO(rt)<<"take_one_task_and_run() id:"<<get_thrd_id()<<" get task... "<<pTask.get();)
        pTask->run();
    }
    else {
        b = steal_one_task_and_run();
    }
    return b;
}
//#if 0
void			runtime::thread_run()
{
    bool flag = false;
    thread_local static int cur_id = get_thrd_id();
    _DEBUG(LOG_INFO(rt)<<"runtime::thread_run() id:"<<cur_id<<" enter...")
    while(!m_bAllThreadsQuit)
    {
      _DEBUG(LOG_TRACE(rt)<<"thread run loop, thrd_id:"<<cur_id)
        flag = take_one_task_and_run();
        //if(!flag)
        //    flag = steal_one_task_and_run();
        if(!flag)
            yield();
    }
}

bool		runtime::steal_one_task_and_run()
{
    thread_local static int cur_id = get_thrd_id();
    size_t dis = 1;
    bool b = false;
    size_t ts = m_oQueues.size();
    task_base_ptr pTask;
    while((cur_id + dis)%ts !=cur_id)
    {
      _DEBUG(LOG_TRACE(rt)<<"try to steal task, thrd_id: "<<cur_id<<", victim id:"<<((cur_id + dis)%ts))
        if(m_oQueues[(cur_id + dis)%ts]->steal(pTask))
        {
            _DEBUG(LOG_INFO(rt)<<"thread id:"<<cur_id<<"stole a task "<< pTask.get());
            pTask->run();
            b = true;
            break;
        }
        dis ++;
    }
    return b;
}

bool		runtime::is_idle()
{
    thread_local static int cur_id = get_thrd_id();
    thread_local static size_t ts = m_oQueues.size();
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
  thread_local static int cur_id = get_thrd_id();
    thread_local static size_t ts = m_oQueues.size();
    size_t dis = 1;
    thrd_id_t idle_id = (cur_id + 1) %ts;
    auto idle_queue_size = m_oQueues[(cur_id + 1)%ts]->size();
    while((cur_id + dis) %ts != cur_id)
    {
      auto id = (cur_id + dis)%ts;
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

}//end namespace rt
bool is_idle()
{
    static rt::runtime_ptr r = rt::runtime::instance();
    return r->is_idle();
}
}//end namespace ff