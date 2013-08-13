#ifndef FF_RNTIME_RING_BUFF_H_
#define FF_RNTIME_RING_BUFF_H_
#include "common/common.h"

namespace ff {
namespace internal {

//N , 2^N.
template <class T, size_t N>
class nonblocking_stealing_queue
{
    const static uint64_t INITIAL_SIZE=1<<N;
protected:
    struct meta_data {
        T * array;
        int64_t  head;
        int64_t  tail;
        uint64_t cap;
    };
public:
    nonblocking_stealing_queue()
    {
        meta_data md;
        md.array = new T[1<<N];
        md.head=0;
        md.tail = 0;
        md.cap = 1<<N;
        m.store(md);
    }

    void push_back(const T & val)
    {
        meta_data ld = m.load();
        if(ld.head-ld.tail == ld.cap)
        {   resize(ld.cap<<1);
            ld = m.load();
        }
        auto mask = ld.cap -1;
        ld.array[ld.head&mask] = val;
        ld.head ++;
        m.store(ld);
    }

    bool pop(T & val)
    {
        meta_data ld = m.load();
        if(ld.head == ld.tail)
            return false;
        if(ld.head - ld.tail >= ld.cap>>2 &&
                ld.head-ld.tail > INITIAL_SIZE)
        {
            resize(ld.cap>>1);
            ld = m.load();
        }
        auto mask = ld.cap - 1;
        auto p = ld.head -1;
        ld.head --;
        m.store(ld);
        val = ld.array[p&mask];
        return true;
    }

    bool steal(T & val)
    {
        while(1)
        {
            meta_data ld = m.load();
            if(ld.head == ld.tail)
                return false;
            auto mask = ld.cap - 1;
            val = ld.array[ld.tail & mask];
            meta_data ep = ld;
            ep.tail ++;
            if(m.compare_exchange_strong(&ld, ep))
                return true;
        }
    }
protected:
    void		resize(uint64_t s)
    {
        auto c1 = new T[s];
        auto tm = m.load();
        auto mask = tm.cap-1;
        int64_t j = 0;
        for(int64_t i = tm.tail, j = 0; i< tm.head; ++i, ++j)
        {
            c1[j] = tm.array[i&mask];
        }
        tm.array = c1;
        tm.tail = 0;
        tm.head = j;
        tm.cap = s;
        m.store(tm);
    }
protected:
    std::atomic<meta_data> m;
};//end class nonblocking_stealing_queue

}//end namespace internal
}//end namespace ff
#endif
