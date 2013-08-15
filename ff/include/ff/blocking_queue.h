#ifndef FF_COMMON_BLOCKING_QUEUE_H_
#define FF_COMMON_BLOCKING_QUEUE_H_
#include <mutex>
#include <atomic>
#include <condition_variable>
#include <queue>

namespace ff {
template<class Ty>
class blocking_queue
{
public:
	blocking_queue(const blocking_queue<Ty> & ) = delete;
	blocking_queue<Ty> operator =(const blocking_queue<Ty> &) = delete;
    blocking_queue() 
    : m_oMutex()
    , m_oCond()
    , m_bForceQuit(false)
    , m_oContainer(){ }

    void push_back(const Ty & val)
    {
        m_oMutex.lock();
        m_oContainer.push(val);
        size_t s = m_oContainer.size();
        m_oMutex.unlock();

        if(s == 1)
        {
            m_oCond.notify_all();
        }
    }

    bool pop(Ty & val)
    {
        std::unique_lock<std::mutex> ul(m_oMutex);

        while(m_oContainer.empty() && !m_bForceQuit)
        {
            m_oCond.wait(ul);
        }

        if(m_bForceQuit)
	  return false;
        val = m_oContainer.front();
        m_oContainer.pop();
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
    
    void force_quit()
    {
      m_bForceQuit = true;
      m_oCond.notify_all();
    }
protected:
	mutable std::mutex  m_oMutex;
	mutable std::condition_variable m_oCond;
	mutable std::atomic_bool	m_bForceQuit;
    std::queue<Ty> m_oContainer;
    
};

}//end namespace ff

#endif
