#include "ff.h"
#include "common/fsetjmp.h"


#include "common/log.h"
#include <iostream>

using namespace ff;
int main()
{
    _DEBUG(ff::log<>::init(ff::INFO, "log.txt"))
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