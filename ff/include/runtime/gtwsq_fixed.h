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

#ifndef FF_RUNTIME_GTWSQ_FIXED_H_
#define FF_RUNTIME_GTWSQ_FIXED_H_
#include <cassert>


namespace ff{
namespace rt{

//N, 2^N.
template <class T, size_t N>
class gcc_work_stealing_queue
{
    const static int64_t INITIAL_SIZE=1<<N;
    const static int64_t mask = (1<<N) - 1;
public:
    gcc_work_stealing_queue()
      : head(0)
      , tail(0)
      , array(new T[1<<N])
        {
        if(array == nullptr)
        {
          assert(false && "Allocation Failed!");
          exit(-1);
        }
      }

    ~gcc_work_stealing_queue()
    {
      //std::cout<<p1<<", "<<p2<<", "<<p3<<", "<<p4<<std::endl;
      if(array != nullptr)
      {
        delete[] array;
        array = nullptr;
      }
    }

    bool push_back(const T & val)
    {
      auto h = head;
      if(h - tail == mask) return false;
      array[h & mask] = val;
      head = h + 1;
      return true;
    }

    bool pop(T & val)
    {
      if(head <= tail) {head = tail; return false;}

      head --;
      //if(head -tail < 100)
        __sync_synchronize(); //This two lines are the magic
      auto t = tail;
      if(head<t)
      {
        head= tail;
        return false;
      }
      val = array[head&mask];
      if(head > t) return true;
      bool res = true;
      if(!__sync_bool_compare_and_swap(&tail, head, head+1)) res = false;
      head = tail;
      return res;
    }

    bool steal(T & val)
    {
      int64_t t = tail;
      int s = head - t;
      if ( s <= 0){
        return false;
      }
      val = array[t&mask];
      if(!__sync_bool_compare_and_swap(&tail, t, t+1)) return false;
      return true;
    }


    int64_t size()
    {
      return (head- tail);
    }
    int64_t  get_head() const {return head;}
    int64_t  get_tail() const {return tail;}
protected:
    int64_t head;
    int64_t tail;
    T *       array;
};//end class classical_work_stealing_queue

}//end namespace rt
}//end namespace ff
#endif

