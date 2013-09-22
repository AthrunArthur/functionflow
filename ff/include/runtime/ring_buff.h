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
        : array(new T[1<<N])
        , head(0)
        , tail(0)
        , cap(1<<N)
        , thieves(0)
        , is_resizing(false)
	, m_hp()
    {
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
        auto t = tail.load();
        auto h = head.load();
        auto c = cap.load();
        auto a = array.load();
        if(h - t == c)
        {
            resize(c<<1);
            t = tail.load();
            h = head.load();

            c = cap.load();
            a = array.load();
        }
        auto mask = c -1;
        a[h&mask] = val;
        _DEBUG(LOG_TRACE(queue)<<"mask:"<<mask<<" pos:"<<(h&mask));
        head.store(h+1);
    }

    bool pop(T & val)
    {
        auto t = tail.load();
        auto h = head.load();
        auto c = cap.load();
        auto a = array.load();
	auto h_h = h;
	auto t_h = t;
	
        if(h == t)
            return false;

        if(h - t <= c>>2 &&
                h - t > INITIAL_SIZE)
        {
            resize(c>>1);
            t = tail.load();
            h = head.load();

            c = cap.load();
            a = array.load();
        }
        std::atomic<T *> & hp = m_hp.get_hazard_pointer();
        scope_guard _sg([]() {}, [&hp]() {
            hp.store(nullptr);
        });

        auto mask = c - 1;
        auto pos = h - 1;
        hp.store(&a[pos & mask]);

        if(h - t-2<=thieves &&
                m_hp.outstanding_hazard_pointer_for(hp.load()))
        {
            return false;
        }
        t = tail.load();
        if(h == t)
        {
            return false;
        }

        head.store(pos);
	
        val = a[pos&mask];
        _DEBUG(LOG_TRACE(queue)<<"mask:"<<mask<<" pos:"<<(pos&mask));
        return true;
    }

    bool steal(T & val)
    {
        while(is_resizing.load()) yield();
        scope_guard _sg([this]() {
            thieves ++;
        }, [this]() {
            thieves --;
        });

	/*
        auto c = cap.load(std::memory_order_acquire);
        auto a = array.load(std::memory_order_relaxed);
        auto mask = c - 1;

        steal_lock.lock();
        auto t = tail.load(std::memory_order_acquire);
        if(t == head.load(std::memory_order_acquire))
        {
            steal_lock.unlock();
            return false;
        }
        T * p = &(a[t&mask]);
        tail.store(t + 1, std::memory_order_release);
        steal_lock.unlock();
        val = *p;
        return true;
	*/
	
	auto t = tail.load();
        auto h = head.load();
        auto c = cap.load();
        auto a = array.load();

	auto h_h = h;
	auto t_h = t;
	
        if(t == h)
            return false;
        std::atomic<T *> & hp = m_hp.get_hazard_pointer();
	scope_guard _sg1([]() {}, [&hp]() {
            hp.store(nullptr);
        });
	
        auto mask = cap-1;
        do {
            h = head.load();
            t = tail.load();
            hp.store(&a[t&mask]);
        } while(h> t && m_hp.outstanding_hazard_pointer_for(hp.load()));

	
        if(m_hp.outstanding_hazard_pointer_for(hp.load()) )
            return false;

	h = head.load();
	if(h == t)
	  return false;
        val = *(hp.load());
        tail.store(t+1);
        return true;
	
    }

    uint64_t	size()
    {
        return head.load() - tail.load();
    }
protected:
    void		resize(uint64_t s)
    {
        _DEBUG(LOG_INFO(queue)<<"enter! head:"<<head.load()<<" tail:"<<tail.load()<<" origin size:"<<cap.load()<<" -->"<<s);
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

        is_resizing.store(true);
        while(thieves.load() != 0) yield();
        auto t2 = tail.load();
        array.store(c1);
        tail.store(t2 - old_tail);
        head.store( j);
        cap.store(s);
        is_resizing.store(false);

        delete[] temp;
        _DEBUG(LOG_INFO(queue)<<"exit! head:"<<head.load()<<" tail:"<<tail.load()<<" origin size:"<<cap.load()<<" -->"<<s);
    }
protected:
    std::atomic_llong  head;
    std::atomic<T *> array;
    std::atomic_ullong cap;
    std::atomic<bool> is_resizing;

    std::atomic<int> thieves;

    std::atomic_llong  tail;
    ff::rt::hp_owner<T> m_hp;
    //ff::spinlock		steal_lock;
};//end class nonblocking_stealing_queue

}//end namespace rt
}//end namespace ff
#endif
