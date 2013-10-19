#ifndef FF_COMMON_SPIN_LOCK_H_
#define FF_COMMON_SPIN_LOCK_H_
#include "common/common.h"
#include "runtime/rtcmn.h"
namespace ff{
  class spinlock
  {
    std::atomic_flag flag;
  public:
    spinlock()
    : flag(ATOMIC_FLAG_INIT){}
    
    spinlock(const spinlock & ) = delete;
    spinlock & operator=(const spinlock &) = delete;
    
    inline void lock(){
      while(flag.test_and_set(std::memory_order_acquire)) ff::rt::yield();
    }
    
    inline bool try_lock(){
      return !flag.test_and_set(std::memory_order_acquire);
    }
    inline void unlock(){
      flag.clear(std::memory_order_release);
    }
  };//end class spin_lock
}//end namespace ff

#endif