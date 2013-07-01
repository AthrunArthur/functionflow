#ifndef FF_RUNTIME_THREAD_POOL_H_
#define FF_RUNTIME_THREAD_POOL_H_
#include <thread>
#include <vector>
#include "runtime/task_queue.h"
#include <memory>
namespace ff {
namespace rt
{
class threadpool
{
public:
	threadpool();
	
	template<class FT>
	void		run(int thrd_num, FT && f)
	{
		for(int i = 0; i<thrd_num; ++i)
		{
			m_oThreads.push_back(std::make_shared<std::thread>([](){
				std::thread::id id = std::this_thread::get_id();
				f(id);
			}));
		}
	}
	void join();
	
protected:
	std::vector<std::shared_ptr<std::thread>>	m_oThreads;
};//end class threadpool;
}//end namespace rt
}//end namespace ff

#endif
