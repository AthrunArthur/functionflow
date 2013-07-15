#include "runtime/env.h"
#include "runtime/rtcmn.h"
#include "runtime/runtime.h"
//#include "common/log.h"
#include <functional>
#include <thread>
#include <iostream>
namespace ff {
namespace rt {

std::shared_ptr<runtime_deletor> runtime_deletor::s_pInstance(nullptr);
runtime_ptr runtime::s_pInstance(nullptr);
std::once_flag		runtime::s_oOnce;
thread_local work_stealing_queue_ptr runtime::m_pLQueue(nullptr);

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
    int thrd_num = std::thread::hardware_concurrency();
    for(int i = 0; i<thrd_num; ++i)
    {
        s_pInstance->m_oQueues.push_back(std::unique_ptr<work_stealing_queue>(new work_stealing_queue()));
    }
  //  LOG_INFO(thread)<<"runtime::init, thread num:"<<thrd_num;
    for(int i = 0; i< thrd_num; ++i)
	{
		s_pInstance->m_pTP->run([i](){
			auto r = runtime::instance();
			m_pLQueue = r->m_oQueues[i].get();
			r->thread_run(i);
		});
	}
    //LOG_INFO(thread)<<"runtime::init over"<<thrd_num;
}
}//end namespace rt
}//end namespace ff
