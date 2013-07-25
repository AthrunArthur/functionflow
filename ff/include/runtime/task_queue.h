#ifndef FF_RUNTIME_TASK_QUEUE_H_
#define FF_RUNTIME_TASK_QUEUE_H_
#include "ff/blocking_queue.h"
#include "ff/nonblocking_queue.h"
#include "runtime/taskbase.h"
#include "runtime/ring_buff.h"
#include <deque>
namespace ff {
namespace rt {
typedef ff::nonblocking_queue<task_base_ptr> task_queue;
#if 0
	typedef internal::ring_buff<task_base_ptr, 64> task_block;
	typedef internal::ring_buff<task_base_ptr, 128> local_task_block;
	typedef std::shared_ptr<task_block> task_block_ptr;
	
//This is for the global queue.
class task_queue
{
public:
	task_queue()
	: pCurrent(nullptr)
	, m_oContainer(){}
	
	void push_back(const task_base_ptr & val)
    {
		if(!pCurrent || pCurrent->is_full())
		{
			atomic_generate_current_ptr();
		}
		std::unique_lock<std::mutex> ul(m_oMutex);
		bool b = pCurrent->push_front(val);
		assert(b);
    }

    bool pop(task_base_ptr & val)
    {
		if(!pCurrent)
			return false;
		std::unique_lock<std::mutex> ul(m_oMutex);
		std::cout<<"pop 1"<<std::endl;
		if(pCurrent->pop_front(val))
		{
			std::cout<<"pop 2"<<std::endl;
			return true;
		}
		std::cout<<"pop 3 "<<std::endl;
		task_block_ptr t;
		if(m_oContainer.pop(t))
		{
			std::cout<<"pop 4"<<std::endl;
			pCurrent = t;
			return pCurrent->pop_front(val);
		}
		return false;
    }

    size_t size() const
    {
		if(pCurrent)
			return m_oContainer.size() * task_block::SIZE + pCurrent->size();
		else 
			return 0;
    }
    bool empty() const
    {
		if(pCurrent)
			return m_oContainer.empty() && pCurrent->is_empty();
		else
			return m_oContainer.empty();
    }
    bool steal_block(task_block_ptr & p)
	{
		if(m_oContainer.empty())
			return false;
		if(m_oContainer.size() == 1)
			return false;
		return m_oContainer.pop(p);
	}
protected:
	void		atomic_generate_current_ptr()
	{
		if(!pCurrent)
		{
			std::unique_lock<std::mutex> ul(m_oMutex);
			if(pCurrent)return;
			pCurrent = std::make_shared<task_block>();
			m_oContainer.push_back(pCurrent);
		}
	}
protected:
	task_block_ptr 	pCurrent;
	std::mutex		m_oMutex;
	ff::nonblocking_queue<task_block_ptr> m_oContainer;
};//end class task_queue


class work_stealing_queue
{
public:
	work_stealing_queue(const work_stealing_queue & ) = delete;
	work_stealing_queue operator =(const work_stealing_queue &) = delete;
    work_stealing_queue() { }

    void push_back(const task_base_ptr & val)
    {
		m_oContainer.push_front(val);
    }

    bool pop(task_base_ptr & val)
    {
		return m_oContainer.pop_front(val);
    }
    
    bool steal(task_base_ptr &val)
	{
		return m_oContainer.pop_back(val);
	}

    size_t size() const
    {
        return m_oContainer.size();
    }
    bool empty() const
    {
        return m_oContainer.is_empty();
    }
    local_task_block &  internal_tasks(){return m_oContainer;}
protected:
	local_task_block	m_oContainer;
};
#endif
#if 1
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

#endif
typedef work_stealing_queue * work_stealing_queue_ptr;
}//end namespace rt
}//end namespace ff
#endif