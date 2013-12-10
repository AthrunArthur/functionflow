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

#define MEM_SEQ_CST std::memory_order_seq_cst
  /*
#define MEM_ACQUIRE std::memory_order_seq_cst
#define MEM_RELAXED std::memory_order_seq_cst
#define MEM_RELEASE std::memory_order_seq_cst
*/

#define MEM_ACQUIRE std::memory_order_acquire
#define MEM_RELAXED std::memory_order_relaxed
#define MEM_RELEASE std::memory_order_release

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
            delete[] array.load(MEM_ACQUIRE);
        }
    }

    void push_back(const T & val)
    {
        auto t = tail.load(MEM_ACQUIRE);
        auto h = head.load(MEM_RELAXED);
        auto c = cap.load(MEM_RELAXED);
        auto a = array.load(MEM_RELAXED);
        if(h - t == c-1)
        {
            resize(c<<1);
            t = tail.load(MEM_ACQUIRE);
            h = head.load(MEM_RELAXED);

            c = cap.load(MEM_RELAXED);
            a = array.load(MEM_RELAXED);
        }
        auto mask = c -1;
        a[h&mask] = val;
        _DEBUG(LOG_TRACE(queue)<<"mask:"<<mask<<" pos:"<<(h&mask));
        head.store(h+1, MEM_RELEASE);
    }

    bool pop(T & val)
    {
        auto t = tail.load(MEM_ACQUIRE);
        auto h = head.load(MEM_RELAXED);
        auto c = cap.load(MEM_RELAXED);
        auto a = array.load(MEM_RELAXED);

        if(h == t)
            return false;

        if(h - t <= c>>2 &&
                h - t > INITIAL_SIZE)
        {
            resize(c>>1);
            t = tail.load(MEM_ACQUIRE);
            h = head.load(MEM_RELAXED);

            c = cap.load(MEM_RELAXED);
            a = array.load(MEM_RELAXED);
        }
        std::atomic<T *> & hp = m_hp.get_hazard_pointer();
        scope_guard _sg([]() {}, [&hp]() {
            hp.store(nullptr, MEM_SEQ_CST);
        });

        auto mask = c - 1;
        auto pos = h - 1;
        hp.store(&a[pos & mask], MEM_SEQ_CST);

	t = tail.load(MEM_ACQUIRE);
        if(h - t<=(thieves.load(MEM_ACQUIRE) <<1) &&
                m_hp.outstanding_hazard_pointer_for(hp.load(MEM_SEQ_CST)))
        {
            return false;
        }
        t = tail.load(MEM_ACQUIRE);
        if(h == t)
        {
            return false;
        }

        head.store(pos, MEM_RELEASE);

        val = a[pos&mask];
        _DEBUG(LOG_TRACE(queue)<<"mask:"<<mask<<" pos:"<<(pos&mask));
        return true;
    }


    bool steal(T & val)
    {
        while(is_resizing.load(MEM_ACQUIRE)) yield();
        scope_guard _sg([this]() {
            thieves ++;
        }, [this]() {
            thieves --;
        });
	if(is_resizing.load(MEM_ACQUIRE))  //We hate ABA problems
	  return false;
	
	if(thieves.load(MEM_ACQUIRE) >=2 )
	  return false;
	
        long long t = 0;
        long long  h = 0;
        auto c = cap.load(MEM_ACQUIRE);
        auto a = array.load(MEM_ACQUIRE);

//        if(t == h)
//            return false;
        std::atomic<T *> & hp = m_hp.get_hazard_pointer();
        scope_guard _sg1([]() {}, [&hp]() {
            hp.store(nullptr, MEM_SEQ_CST);
        });

        auto mask = cap-1;

        bool ready = false;
	int i = 1;
        while(!ready)
        {
            i++;
	    if(i>16)//3) //here is an experience value!
	      return false;

            do {
                _DEBUG(
                    i ++;
                    if(i > 10000)
                    LOG_FATAL(queue)<<"stuck here! head: "<<head.load()<<", tail: "<<tail.load()<<", hp:"<<m_hp.str();
                )
                h = head.load(MEM_ACQUIRE);
                t = tail.load(MEM_ACQUIRE);
                if(!m_hp.outstanding_hazard_pointer_for(&a[t&mask]))
                    hp.store(&a[t&mask], MEM_SEQ_CST);
                else
                    hp.store(nullptr, MEM_SEQ_CST);
            } while(h> t && m_hp.outstanding_hazard_pointer_for(hp.load(MEM_SEQ_CST)));


            if(hp.load(MEM_SEQ_CST) == nullptr ||
                    m_hp.outstanding_hazard_pointer_for(hp.load(MEM_ACQUIRE)) )
                //return false;
	      continue;

            h = head.load(MEM_ACQUIRE);
            if(h == t)
                return false;
            val = *(hp.load(MEM_SEQ_CST));
            if(tail.compare_exchange_strong(t, t+1, std::memory_order_acq_rel))
	    {
	      return true;
	    }
	    //else
	    //  return false;
        }
        return false;
    }

    uint64_t	size()
    {
        return head.load(MEM_ACQUIRE) - tail.load(MEM_ACQUIRE);
    }
protected:
    void		resize(uint64_t s)
    {
        _DEBUG(LOG_INFO(queue)<<"enter! head:"<<head.load()<<" tail:"<<tail.load()<<" origin size:"<<cap.load()<<" -->"<<s);
        auto c1 = new T[s];
        auto mask = cap.load(MEM_RELAXED)-1;

        auto old_tail = tail.load(MEM_ACQUIRE);
        int64_t i = old_tail, j = 0;
        auto h = head.load(MEM_RELAXED);
        T * temp = array.load(MEM_RELAXED);
        for( j = 0, i = old_tail; i< h; ++i, ++j)
        {
            c1[j] = temp[i&mask];
        }

        is_resizing.store(true, MEM_RELEASE);
	while(thieves.load(MEM_ACQUIRE) != 0) yield();
        
	auto t2 = tail.load(MEM_ACQUIRE);
        array.store(c1, MEM_RELEASE);
        cap.store(s, MEM_RELEASE);
        tail.store(t2 - old_tail, MEM_RELEASE);
        head.store( j, MEM_RELEASE);

        is_resizing.store(false, MEM_RELEASE);

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
