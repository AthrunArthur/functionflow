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
#ifndef FF_COMMON_COMMON_H_
#define FF_COMMON_COMMON_H_

#include <cstdint>
#include <functional>
#include <atomic>
#include <thread>
#include <type_traits>
#include <iterator>
#include <cassert>

#define CACHE_LINE_SIZE 64
#define FF_DEFAULT_PARTITIONER simple_partitioner //or auto_partitioner
//#define RECORD_WORK_STEAL //This is the on-off switch for logging work-stealing behavior
//#define FUNCTION_FLOW_DEBUG


#ifdef FUNCTION_FLOW_DEBUG
#include <iostream>
#endif

namespace ff {

enum exe_state {
    exe_empty = 1,
    exe_init,
    exe_wait,
    exe_over,
    exe_run,
};
exe_state operator &&(exe_state e1, exe_state  e2);
exe_state operator ||(exe_state e1, exe_state e2);

typedef void *  mutex_id_t;
const mutex_id_t invalid_mutex_id = NULL;

typedef int32_t thrd_id_t;
const thrd_id_t invalid_thrd_id = -1;

}//end namespace ff


#endif
