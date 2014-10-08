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
#ifndef FF_COMMON_THREAD_LOCAL_STORAGE_H_
#define FF_COMMON_THREAD_LOCAL_STORAGE_H_
#include "common/common.h"
#include "common/spin_lock.h"

#include <unordered_map>


namespace ff{

    //!It seems clang still doesn't support C++11's thread_local, thus we use this to replace thread_local
    template <class T>
    class thread_local_storage{
    public:
        thread_local_storage(const thread_local_storage<T> & ) = delete;
        thread_local_storage<T > & operator = (const thread_local_storage<T> & ) = delete;
        thread_local_storage()
                : m_oData(){}

        void    set(const T & t)
        {
            m_oLock.lock();
            m_oData[std::this_thread::get_id()] = t;
            m_oLock.unlock();
        }
        const T &     get() const
        {
            m_oLock.lock();
            typename TLSMap_t::const_iterator it = m_oData.find(std::this_thread::get_id());
            assert(it != m_oData.end() && "Cannot find the thread local storaged data!");

            const T & t = it->second;
            m_oLock.unlock();
            return t;
        }
    protected:
        typedef std::unordered_map<std::thread::id, T> TLSMap_t;
        TLSMap_t  m_oData;
        mutable ff::spinlock m_oLock;
    };//end class thread_local_storage
}//end namespace ff

#endif