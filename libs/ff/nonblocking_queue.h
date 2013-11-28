/*
 The MIT License (MIT)
Copyright (c) 2013 <AthrunArthur>
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
*/

#ifndef FF_COMMON_NONBLOCKING_QUEUE_H_
#define FF_COMMON_NONBLOCKING_QUEUE_H_
#if __cplusplus < 201103L
#include <boost/thread/mutex.hpp>
#include <boost/noncopyable.hpp>
#else
#include <mutex>
#endif

namespace ff {
template<class Ty>
class nonblocking_queue
#if __cplusplus < 201103L
: public boost::noncopyable
#endif
{
public:
#if __cplusplus >= 201103L
    nonblocking_queue(const nonblocking_queue<Ty> & ) = delete;
    nonblocking_queue<Ty> operator =(const nonblocking_queue<Ty> &) = delete;
#endif
    nonblocking_queue() { }

    void push_back(const Ty & val)
    {
#if __cplusplus < 201103L
        boost::unique_lock<boost::mutex> ul(m_oMutex);
#else
        std::unique_lock<std::mutex> ul(m_oMutex);
#endif
        m_oContainer.push(val);
    }

    bool pop(Ty & val)
    {
#if __cplusplus < 201103L
        boost::unique_lock<boost::mutex> ul(m_oMutex);
#else
        std::unique_lock<std::mutex> ul(m_oMutex);
#endif

        if(m_oContainer.empty())
            return false;
        val = std::move(m_oContainer.front());
        m_oContainer.pop();
        return true;
    }

    size_t size() const
    {
#if __cplusplus < 201103L
        boost::unique_lock<boost::mutex> ul(m_oMutex);
#else
        std::unique_lock<std::mutex> ul(m_oMutex);
#endif
        return m_oContainer.size();
    }
    bool empty() const
    {
#if __cplusplus < 201103L
        boost::unique_lock<boost::mutex> ul(m_oMutex);
#else
        std::unique_lock<std::mutex> ul(m_oMutex);
#endif
        return m_oContainer.empty();
    }
protected:
#if __cplusplus < 201103L
    mutable boost::mutex m_oMutex;
#else
    mutable std::mutex  m_oMutex;
#endif
    std::queue<Ty> m_oContainer;
};


}//end namespace ff
#endif
