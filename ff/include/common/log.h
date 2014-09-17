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
#ifndef COMMON_LOG_H_
#define COMMON_LOG_H_

#ifdef FUNCTION_FLOW_DEBUG
#define SYNC_WRITING_LOG
#include "ff/fflog.h"
#endif



#ifdef USING_FF_LOG
#define _DEBUG(stmt) stmt;
#else
#define _DEBUG(stmt)
#define DEF_LOG_MODULE(m) 
#define ENABLE_LOG_MODULE(m)
#endif

DEF_LOG_MODULE(main)
DEF_LOG_MODULE(para)
DEF_LOG_MODULE(rt)
DEF_LOG_MODULE(queue)

ENABLE_LOG_MODULE(main)

ENABLE_LOG_MODULE(para)

ENABLE_LOG_MODULE(rt)

ENABLE_LOG_MODULE(queue)

#endif
