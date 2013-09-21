#ifndef FF_RUNTIME_HAZARD_POINTER_H_
#define FF_RUNTIME_HAZARD_POINTER_H_
#include "runtime/env.h"
#include <mutex>
#include <boost/concept_check.hpp>

//! This hazard pointer is specifical for FF!!
namespace ff {
namespace rt
{
template<class T>
class hp_owner
{
public:
    hp_owner()
    : m_oflag(){
        std::call_once(m_oflag, [this]() {
            m_pPointers = new std::atomic<T *>[ff::rt::rt_concurrency()];
        });
    }

    ~hp_owner(){
      delete [] m_pPointers;
    }
    
    std::atomic<T *> & get_hazard_pointer() {
        return m_pPointers[ff::rt::get_thrd_id()];
    }

    bool  outstanding_hazard_pointer_for(T * p)
    {
        thread_local static thrd_id_t id = ff::rt::get_thrd_id();
        if(!p)
            return false;
        for(int i = 0; i < ff::rt::rt_concurrency(); i++)
        {
            if(i == id)
                continue;
            if(m_pPointers[i].load(std::memory_order_acquire) == p)
                return true;
        }
        return false;
    }
protected:
    std::once_flag m_oflag;
    std::atomic<T *>*  m_pPointers;
};//end class hp_owner

}//end namespace rt
}//end namespace ff


#endif