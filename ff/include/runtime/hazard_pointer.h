#ifndef FF_RUNTIME_HAZARD_POINTER_H_
#define FF_RUNTIME_HAZARD_POINTER_H_
#include "runtime/env.h"
#include <mutex>

//! This hazard pointer is specifical for FF!!
namespace ff {
namespace rt
{
template<class T>
class hp_owner
{
public:
    hp_owner()
        : m_pPointer(nullptr) {
        std::call_once(s_oflag, []() {
            s_pPointers = new std::atomic<T *>[ff::rt::rt_concurrency()];
        });
        m_pPointer = & s_pPointers[ff::rt::get_thrd_id()];
    }

    std::atomic<T *> & get_hazard_pointer() {
        return *m_pPointer;
    }

    static bool  outstanding_hazard_pointer_for(T * p)
    {
        thread_local static thrd_id_t id = ff::rt::get_thrd_id();
        if(!p)
            return false;
        for(int i = 0; i < ff::rt::rt_concurrency(); i++)
        {
            if(i == id)
                continue;
            if(s_pPointers[i].load(std::memory_order_acquire) == p)
                return true;
        }
        return false;
    }
protected:
    static std::once_flag s_oflag;
    static std::atomic<T *>*  s_pPointers;
    std::atomic<T *> * m_pPointer;
};//end class hp_owner

template<class T>
std::once_flag hp_owner<T>::s_oflag;
template<class T>
std::atomic<T *> * hp_owner<T>::s_pPointers = nullptr;

template <class T>
std::atomic<T *> &  get_hazard_pointer_for_cur_thrd()
{
    thread_local static hp_owner<T> hp;
    return hp.get_hazard_pointer();
}

template <class T>
bool outstanding_hazard_pointer_for( T * p)
{
    return hp_owner<T>::outstanding_hazard_pointer_for(p);
}
}//end namespace rt
}//end namespace ff


#endif