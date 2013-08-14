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
	delete[] array;
      }
    }

    void push_back(const T & val)
    {
      std::atomic_thread_fence(std::memory_order_acquire);
        if(head-tail == cap)
        {   
	  resize(cap<<1);
	  std::atomic_thread_fence(std::memory_order_release);
        }
        auto mask = cap -1;
        array[head&mask] = val;
        head ++;
        std::atomic_thread_fence(std::memory_order_release);
    }

    bool pop(T & val)
    {
        std::atomic_thread_fence(std::memory_order_acquire);
        if(head == tail)
            return false;
        if(head - tail >= cap>>2 &&
                head-tail > INITIAL_SIZE)
        {
            resize(cap>>1);
            std::atomic_thread_fence(std::memory_order_release);
        }
        auto mask = cap - 1;
        auto p = head -1;
        head --;
        std::atomic_thread_fence(std::memory_order_release);
        val = array[p&mask];
	
        return true;
    }

    bool steal(T & val)
    {
      BEGIN:
      std::atomic_thread_fence(std::memory_order_acquire);
      uint64_t l = version.load();
      if(l & 1 || head == tail)
	return false;
      auto mask = cap - 1;
      if(!version.compare_exchange_strong(l, l + 1))
	goto BEGIN;
      val = array[tail % mask];
      tail ++;
      std::atomic_thread_fence(std::memory_order_release);
      auto el = l + 1;
      auto b = version.compare_exchange_strong(el, l + 2);
      assert(b && "should always be true");
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
        auto temp = array;
        array = c1;
        tail = 0;
        head = j;
        cap = s;
	b = version.compare_exchange_strong(l, l + 2);
	assert(b && "Should always be true!");
	delete temp;
    }
protected:
  const static uint64_t VER_MASK=~1;
  std::atomic<uint64_t> version;
  
    T * array;
    int64_t  head;
    int64_t  tail;
    uint64_t cap;
};//end class nonblocking_stealing_queue

}//end namespace rt
}//end namespace ff
#endif
