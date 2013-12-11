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
#ifndef FF_RUNTIME_ENV_H_
#define FF_RUNTIME_ENV_H_
#include "common/common.h"
#include "common/fsetjmp.h"
#include <memory>
#include <vector>
#include <functional>

namespace ff {
namespace rt {

inline size_t  hardware_concurrency(){
	static int t = std::thread::hardware_concurrency() - 1;
	return t;
}
inline size_t rt_concurrency()
{
	return hardware_concurrency() + 1;
}

thrd_id_t get_thrd_id();

void set_local_thrd_id(thrd_id_t i);

  struct ctx_pdict{
    ::ff::jmp_buf_ptr ctx;
    std::function<bool ()> pdict;
  };
  typedef ctx_pdict * ctx_pdict_ptr;
  
  template <class F>
ctx_pdict_ptr make_ctx_pdict_ptr(  F && f)
{
  ctx_pdict_ptr p = new ctx_pdict();
  p->ctx = make_shared_jmp_buf();
  p->pdict = f;//std::bind([f](){f();});
  return p;
}

inline void free_ctx_pdict_ptr(ctx_pdict_ptr p)
{
  delete p;
}


}//end namespace rt
}//end namespace ff


#endif
