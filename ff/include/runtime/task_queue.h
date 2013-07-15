#ifndef FF_RUNTIME_TASK_QUEUE_H_
#define FF_RUNTIME_TASK_QUEUE_H_
#include "ff/blocking_queue.h"
#include "ff/nonblocking_queue.h"
#include "runtime/taskbase.h"
#include <deque>
namespace ff {
namespace rt {
typedef ff::nonblocking_queue<task_base_ptr> task_queue;

class work_stealing_queue
{
public:
	work_stealing_queue(const work_stealing_queue & ) = delete;
	work_stealing_queue operator =(const work_stealing_queue &) = delete;
    work_stealing_queue() { }

    void push_back(const task_base_ptr & val)
    {
		std::unique_lock<std::mutex> ul(m_oMutex);
		m_oContainer.push_back(std::move(val));
    }

    bool pop(task_base_ptr & val)
    {
        std::unique_lock<std::mutex> ul(m_oMutex);

		if(m_oContainer.empty())
			return false;
		val = std::move(m_oContainer.front());
		m_oContainer.pop_front();
		return true;
    }
    
    bool steal(task_base_ptr &val)
	{
		std::unique_lock<std::mutex> ul(m_oMutex);

		if(m_oContainer.empty())
			return false;
		val = std::move(m_oContainer.back());
		m_oContainer.pop_back();
		return true;
	}

    size_t size() const
    {
        std::unique_lock<std::mutex> ul(m_oMutex);
        return m_oContainer.size();
    }
    bool empty() const
    {
        std::unique_lock<std::mutex> ul(m_oMutex);
        return m_oContainer.empty();
    }
protected:
	mutable std::mutex  m_oMutex;
    std::deque<task_base_ptr> m_oContainer;
};//end class work_stealing_queue

typedef work_stealing_queue * work_stealing_queue_ptr;
}
}//end namespace ff
#endif