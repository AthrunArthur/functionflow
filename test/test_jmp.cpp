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
#include "ff.h"
#include "common/fsetjmp.h"


#include "common/log.h"
#include <iostream>

using namespace ff;
int main()
{
    _DEBUG(ff::fflog<>::init(ff::INFO, "log.txt"))
    _DEBUG(LOG_INFO(main)<<"main start, id:"<<ff::rt::get_thrd_id());

    jmp_buf_ptr jbp = make_shared_jmp_buf();
    std::cout<<dump_ctx(jbp.get())<<std::endl;
    int ret = setjmp(jbp.get());
    if(ret == 0)
    {
      std::cout<<"setjmp ret: "<<ret<<std::endl;
    std::cout<<dump_ctx(jbp.get())<<std::endl;
    
    std::cout<<"................................."<<std::endl;
    }
    else
    {
      std::cout<<"jmped from else "<<std::endl;
      exit(1);
    }
    jmp_buf jb1;
    if(setjmp(jb1) == 0){
      std::cout<<dump_ctx(jb1)<<std::endl;
    }
    else
    {
      std::cout<<"jmped from else "<<std::endl;
      exit(1);
    }
    longjmp(jbp.get(), 1);
    return  0;
}