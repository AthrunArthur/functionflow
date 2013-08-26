#ifndef FF_RUNTIME_THREAD_POOL_H_
#define FF_RUNTIME_THREAD_POOL_H_
#include <thread>
#include <vector>
#include <memory>
#include <functional>
#include <iostream>

namespace ff {
namespace rt
{
class threadpool
{
public:
	threadpool(): m_oThreads(){};
	
	
	template<class F>
	void run(int thrd_num, F && func)
	{
		for (int i = 0; i< thrd_num ;i++)  
		{
			m_oThreads.push_back(std::thread(func));  
		}
	}
	
	template<class F>
	void run(F && func)
	{
		m_oThreads.push_back(std::thread(func));  
	}
	
	
	void join()
	{
		for(size_t i = 0; i< m_oThreads.size(); ++i)
		{
			if(m_oThreads[i].joinable())
				m_oThreads[i].join();
		}
	}
	
protected:
	std::vector<std::thread >	m_oThreads;
};//end class threadpool;
}//end namespace rt
}//end namespace ff

#endif
