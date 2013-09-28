#ifndef FF_RUNTIME_SPIN_STEALING_QUEUE_H_
#define FF_RUNTIME_SPIN_STEALING_QUEUE_H_
#include "common/common.h"
#include "common/spin_lock.h"
#include "common/log.h"
#include "common/scope_guard.h"
namespace ff
{
namespace rt
{
template <class T, size_t N>
class spin_stealing_queue
{
    const static uint64_t INITIAL_SIZE=1<<N;
public:
    spin_stealing_queue()
        : array(new T[1<<N])
        , head(0)
        , tail(0)
        , cap(1<<N)
	, steal_lock()
    {
    }
    ~spin_stealing_queue()
    {
        if(array != nullptr)
        {
            delete[] array.load(std::memory_order_acquire);
        }
    }

    void push_back(const T & val)
    {
      scope_guard __l([this]() {steal_lock.lock();}, [this]() {steal_lock.unlock();});
        auto t = tail.load(std::memory_order_acquire);
        auto h = head.load(std::memory_order_relaxed);
        auto c = cap.load(std::memory_order_relaxed);
        auto a = array.load(std::memory_order_relaxed);
        if(h - t == c-1)
        {
            resize(c<<1);
            t = tail.load(std::memory_order_acquire);
            h = head.load(std::memory_order_relaxed);

            c = cap.load(std::memory_order_relaxed);
            a = array.load(std::memory_order_relaxed);
        }
        auto mask = c -1;
        a[h&mask] = val;
        _DEBUG(LOG_TRACE(queue)<<"mask:"<<mask<<" pos:"<<(h&mask));
        head.store(h+1, std::memory_order_release);
    }

    bool pop(T & val)
    {
      scope_guard __l([this]() {steal_lock.lock();}, [this]() {steal_lock.unlock();});
        auto t = tail.load(std::memory_order_acquire);
        auto h = head.load(std::memory_order_relaxed);
        auto c = cap.load(std::memory_order_relaxed);
        auto a = array.load(std::memory_order_relaxed);

        if(h == t)
            return false;

        if(h - t <= c>>2 &&
                h - t > INITIAL_SIZE)
        {
            resize(c>>1);
            t = tail.load(std::memory_order_acquire);
            h = head.load(std::memory_order_relaxed);

            c = cap.load(std::memory_order_relaxed);
            a = array.load(std::memory_order_relaxed);
        }
        
        auto mask = c - 1;
        auto pos = h - 1;
        if(h == t)
        {
            return false;
        }

        head.store(pos, std::memory_order_release);

        val = a[pos&mask];
        _DEBUG(LOG_TRACE(queue)<<"mask:"<<mask<<" pos:"<<(pos&mask));
        return true;
    }


    bool steal(T & val)
    {
      scope_guard __l([this]() {steal_lock.lock();}, [this]() {steal_lock.unlock();});
        auto t = tail.load(std::memory_order_acquire);
        auto h = head.load(std::memory_order_acquire);
        auto c = cap.load(std::memory_order_relaxed);
        auto a = array.load(std::memory_order_relaxed);

        if(t == h)
            return false;
        
        auto mask = cap-1;
            val = a[t &mask];
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
        auto mask = cap.load(std::memory_order_relaxed)-1;

        auto old_tail = tail.load(std::memory_order_acquire);
        int64_t i = old_tail, j = 0;
        auto h = head.load(std::memory_order_relaxed);
        T * temp = array.load(std::memory_order_relaxed);
        for( j = 0, i = old_tail; i< h; ++i, ++j)
        {
            c1[j] = temp[i&mask];
        }


        auto t2 = tail.load(std::memory_order_acquire);
        array.store(c1, std::memory_order_relaxed);
        cap.store(s, std::memory_order_relaxed);
        tail.store(t2 - old_tail, std::memory_order_release);
        head.store( j, std::memory_order_release);

        delete[] temp;
        _DEBUG(LOG_INFO(queue)<<"exit! head:"<<head.load()<<" tail:"<<tail.load()<<" origin size:"<<cap.load()<<" -->"<<s);
    }
protected:
    std::atomic_llong  head;
    std::atomic<T *> array;
    std::atomic_ullong cap;
    std::atomic_llong tail;
    ff::spinlock		steal_lock;
};//end class nonblocking_stealing_queue


}//end namespace rt;
}//end namespace ff

#endif