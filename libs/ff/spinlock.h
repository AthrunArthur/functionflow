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
#ifndef FF_SPIN_LOCK_H
#define FF_SPIN_LOCK_H

#if __cplusplus < 201103L
#include <boost/smart_ptr/detail/spinlock.hpp>

namespace ff {
typedef boost::detail::spinlock spinlock;
}//end namespace ff;
#else //C++11

#include <atomic>
namespace ff {
#include <atomic>
	
class spinlock
{
    std::atomic_flag flag;
public:
    spinlock():
        flag(ATOMIC_FLAG_INIT){}
    spinlock(const spinlock & ) = delete;
	spinlock & operator =(const spinlock &) = delete;
	
    inline void lock()
	{
        while(flag.test_and_set(std::memory_order_acquire));
    }
    inline void unlock()
    {
        flag.clear(std::memory_order_release);
    }
};//end class spinlock


}//end namespace ff
#endif//end __cplusplus

#endif //end FF_SPIN_LOCK_H