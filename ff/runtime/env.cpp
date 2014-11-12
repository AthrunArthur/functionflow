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
#include "runtime/env.h"
#include "runtime/rtcmn.h"
#include "runtime/runtime.h"
//#include "common/log.h"
#include <functional>
#include <thread>
#include <iostream>
#include "common/thread_local_storage.h"

namespace ff {
namespace rt {
#ifdef CLANG_LLVM
static ::ff::thread_local_storage<thrd_id_t> s_id;
#else
thread_local static thrd_id_t s_id;
#endif
static size_t max_concurrency = std::thread::hardware_concurrency();//added by sherry

void  set_hardware_concurrency(size_t t){//added by sherry
    if(t <= 0 || t > max_concurrency)
    {
        t = max_concurrency;
    }
    static size_t concurrency = t;//can be changed only once
    max_concurrency = concurrency;
}

size_t  get_hardware_concurrency(){//added by sherry
    return max_concurrency;
}

thrd_id_t get_thrd_id()
{
#ifdef CLANG_LLVM 
    return s_id.get();
#else
    return s_id;
#endif
}

void set_local_thrd_id(thrd_id_t i)
{
#ifdef CLANG_LLVM
    s_id.set(i);
#else
    s_id = i;
#endif
}
}//end namespace rt
}//end namespace ff
