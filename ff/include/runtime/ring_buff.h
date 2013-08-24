#ifndef FF_RNTIME_RING_BUFF_H_
#define FF_RNTIME_RING_BUFF_H_
#include "common/common.h"
#include "common/scope_guard.h"
#include "runtime/hazard_pointer.h"
#include "runtime/rtcmn.h"
#include "common/spin_lock.h"
#include "common/log.h"

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
	
	thieves.store(0);
	is_resizing.store(false);
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
        auto mask = cap.load() -1;
        array[head.load()&mask] = val;
	_DEBUG(LOG_INFO(queue)<<"mask:"<<mask<<" pos:"<<(head.load()&mask));
        head ++;
    }

    bool pop(T & val)
    {
        if(head == tail)
            return false;
	
        if(head - tail <= cap>>2 &&
                head-tail > INITIAL_SIZE)
        {
            resize(cap.load()>>1);
        }
        std::atomic<T *> & hp = get_hazard_pointer_for_cur_thrd<T>();
	scope_guard _sg([](){}, [&hp](){hp.store(nullptr);});
	
	auto mask = cap.load() - 1;
	auto h = head.load() - 1;
	hp.store(&array[h & mask]);
	
	if(head == tail)
	{
	  return false;
	}
	
	if(/*head-tail<=thieves &&*/
	  outstanding_hazard_pointer_for<T>(hp.load()))
	{
	  return false;
	}
        
        head.store(h);
	
        val = array[h&mask];
	_DEBUG(LOG_INFO(queue)<<"mask:"<<mask<<" pos:"<<(h&mask));
        return true;
    }

    bool steal(T & val)
    {
      
      while(is_resizing) yield();
      scope_guard _sg([this](){thieves ++;}, [this](){thieves --;});
      if(head == tail)
	return false;
      std::atomic<T *> & hp = get_hazard_pointer_for_cur_thrd<T>();
      
      uint64_t t;
      auto mask = cap-1;
      do{
	t = tail.load();
	hp.store(&array[t&mask]);
      }while(head> tail && outstanding_hazard_pointer_for<T>(hp.load()));
      
      if(head == tail)
	return false;
      
      val = *(hp.load());
      tail ++;
      return true;
    }
protected:
    void		resize(uint64_t s)
    {
      _DEBUG(LOG_INFO(queue)<<"origin size:"<<cap.load()<<" -->"<<s);
        auto c1 = new T[s];
        auto mask = cap.load()-1;
        
	auto old_tail = tail.load();
	int64_t i = old_tail, j = 0;
	auto h = head.load();
	T * temp = array.load();
        for( j = 0, i = old_tail; i< h; ++i, ++j)
        {
            c1[j] = temp[i&mask];
        }

	is_resizing = true;
	while(thieves.load() != 0) yield();
	auto t2 = tail.load();
        array = c1;
        tail = t2 - old_tail;
        head = j;
        cap = s;
	is_resizing = false;
	
	delete[] temp;
    }
protected:
    std::atomic<T *> array;
    std::atomic_llong  head;
    std::atomic_llong  tail;
    std::atomic_ullong cap;
    std::atomic<int> thieves;
    std::atomic<bool> is_resizing;
};//end class nonblocking_stealing_queue

}//end namespace rt
}//end namespace ff
#endif
