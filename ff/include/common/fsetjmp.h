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
//This will enable a better interface for jmp_buf
#ifndef FF_COMMON_FSETJMP_H_
#define FF_COMMON_FSETJMP_H_
#include <csetjmp>
#include <sstream>
#include <string>
namespace ff{
template<class T>
struct array_len {};

template<class T, size_t N>
struct array_len<T[N]> {
	static const size_t len = N;
};
typedef typename std::remove_extent<std::jmp_buf>::type jmp_buf_primitive;
typedef std::shared_ptr<jmp_buf_primitive> jmp_buf_ptr;


inline jmp_buf_ptr make_shared_jmp_buf()
{
	return jmp_buf_ptr (new jmp_buf_primitive[array_len<jmp_buf>::len], 
					[](jmp_buf_primitive *p){delete p;});
}

std::string dump_ctx(jmp_buf p);
}//end namespace ff
#endif