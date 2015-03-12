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
#include <iostream>
#include <fstream>
#include <sstream>
#include "utils.h"


#define FIBNUM 40
#define MINPARA 25//3
using namespace std;

int64_t sfib(int64_t n )
{
    if(n <= 2)
        return 1;
    return sfib(n -1 ) + sfib(n-2);
}


extern int64_t fib(int64_t);

int main(int argc, char *argv[])
{
  RRecord rr("time.json", "fib");
  ParamParser pp;
  pp.parse(argc, argv);
  int thrd_num = pp.get<int>("thrd-num");
  rr.put("thrd-num", thrd_num);
  if(thrd_num > 0)
    initial_para_env(thrd_num);

    
  start_record_cache_access();
    int64_t num = FIBNUM,fib_res;
    rr.run("elapsed-time", fib, num);

  end_record_cache_access(rr);
    /*Stop papi trace*/
    return 0;
}
