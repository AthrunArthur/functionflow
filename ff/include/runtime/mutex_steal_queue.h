#ifndef FF_RUNTIME_MUTEX_STEALING_QUEUE_H_
#define FF_RUNTIME_MUTEX_STEALING_QUEUE_H_

#include <deque>
#include <mutex>
#include <vector>
#include <memory>

namespace ff
{
namespace rt
{
template<class T>
class mutex_stealing_queue
{
public:
    mutex_stealing_queue(const mutex_stealing_queue<T> & ) = delete;
    mutex_stealing_queue<T> operator =(const mutex_stealing_queue<T> &) = delete;
    mutex_stealing_queue() { }

    void push_back(const T & val)
    {
        std::unique_lock<std::mutex> ul(m_oMutex);
        m_oContainer.push_back(std::move(val));
    }

    bool pop(T & val)
    {
        std::unique_lock<std::mutex> ul(m_oMutex);

        if(m_oContainer.empty())
            return false;
        val = std::move(m_oContainer.back());
        m_oContainer.pop_back();
        return true;
    }

    bool steal(T &val)
    {
        std::unique_lock<std::mutex> ul(m_oMutex);

        if(m_oContainer.empty())
            return false;
        val = std::move(m_oContainer.front());
        m_oContainer.pop_front();
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
    std::deque<T> m_oContainer;
};//end class mutex_stealing_queue


}//end namespace rt;
}//end namespace ff

#endif