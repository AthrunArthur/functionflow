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
#ifndef FF_RUNTIME_RTCMN_H_
#define FF_RUNTIME_RTCMN_H_
#include "common/common.h"
namespace ff {
  namespace rt {

    extern size_t s_hardware_concurrency;
    extern size_t s_current_concurrency;

    inline size_t  hardware_concurrency(){
      return s_hardware_concurrency;
    }

    inline bool set_concurrency(size_t c){
      if(s_current_concurrency == 0){
        if (c == 0) return false;
        s_current_concurrency = c;
        return true;
      }else{return false;}
    }
    inline size_t concurrency()
    {
      if(s_current_concurrency)
        return s_current_concurrency;
      else
        return s_hardware_concurrency;
    }

    thrd_id_t get_thrd_id();

    bool set_local_thrd_id(thrd_id_t i);

    //Give other tasks opportunities to run!
    void yield();
  }//end namespace rt

}//end namespace ff
#endif
