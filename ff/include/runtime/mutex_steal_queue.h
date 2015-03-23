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

    bool push_back(const T & val)
    {
        std::unique_lock<std::mutex> ul(m_oMutex);
        m_oContainer.push_back(std::move(val));
        return true;
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
