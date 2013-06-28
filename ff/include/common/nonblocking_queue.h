#ifndef FF_COMMON_NONBLOCKING_QUEUE_H_
#define FF_COMMON_NONBLOCKING_QUEUE_H_
#include <mutex>
#include <atomic>

namespace ff {
namespace utils {
	template<class Ty>
class nonblocking_queue
{
public:
	nonblocking_queue(const nonblocking_queue<Ty> & ) = delete;
	nonblocking_queue<Ty> operator =(const nonblocking_queue<Ty> &) = delete;
    nonblocking_queue() { }

    void push_back(const Ty & val)
    {
		std::unique_lock<std::mutex> ul(m_oMutex);
		m_oContainer.push(val);
    }

    bool pop(Ty & val)
    {
        std::unique_lock<std::mutex> ul(m_oMutex);

		if(m_oContainer.empty())
			return false;
		val = m_oContainer.front();
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
    std::queue<Ty> m_oContainer;
};

	
}//end namespace utils
}//end namespace ff
#endif
