#ifndef FF_RNTIME_RING_BUFF_H_
#define FF_RNTIME_RING_BUFF_H_
#include "common/common.h"

namespace ff {
namespace rt {

//N , 2^N.
template <class T, size_t N>
class nonblocking_stealing_queue
{
    const static uint64_t INITIAL_SIZE=1<<N;
public:
    nonblocking_stealing_queue()
    {
        array = new T[1<<N];
        head=0;
        tail = 0;
        cap = 1<<N;
	version.store(0);
    }
    ~nonblocking_stealing_queue()
    {
      if(array != nullptr)
      {
	delete[] array.load();
      }
    }

    void push_back(const T & val)
    {
        if(head-tail == cap)
        {   
	  resize(cap.load()<<1);
        }
        auto mask = cap -1;
        array[head&mask] = val;
        head ++;
    }

    bool pop(T & val)
    {
        if(head == tail)
            return false;
        if(head - tail >= cap>>2 &&
                head-tail > INITIAL_SIZE)
        {
            resize(cap.load()>>1);
        }
        auto mask = cap - 1;
        auto p = head -1;
        head --;
        val = array[p&mask];
	
        return true;
    }

    bool steal(T & val)
    {
      BEGIN:
      uint64_t l = version.load();
      if(l & 1 || head == tail)
	return false;
      auto mask = cap - 1;
      if(!version.compare_exchange_strong(l, l + 1))
	goto BEGIN;
      val = array[tail % mask];
      tail ++;
      auto el = l + 1;
      auto b = version.compare_exchange_strong(el, l + 2);
      assert(b && "should always be true");
      return true;
    }
protected:
    void		resize(uint64_t s)
    {
        auto c1 = new T[s];
        auto mask = cap-1;
        int64_t j = 0;
	
	bool b = false;
	uint64_t l = 0;
	while(!b)
	{
	  l = version.load();
	  auto el = l & VER_MASK;
	  b = version.compare_exchange_strong(el, l+1);
	}
	
        for(int64_t i = tail, j = 0; i< head; ++i, ++j)
        {
            c1[j] = array[i&mask];
        }
        auto temp = array.load();
        array = c1;
        tail = 0;
        head = j;
        cap = s;
	auto el = l+1;
	b = version.compare_exchange_strong(el, l + 2);
	assert(b && "Should always be true!");
	delete[] temp;
    }
protected:
  const static uint64_t VER_MASK=~1;
  std::atomic<uint64_t> version;
  
    std::atomic<T *> array;
    std::atomic_llong  head;
    std::atomic_llong  tail;
    std::atomic_ullong cap;
};//end class nonblocking_stealing_queue

}//end namespace rt
}//end namespace ff
#endif
