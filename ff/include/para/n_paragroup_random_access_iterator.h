//!This file is included in paragroup.h
//! this file is only the part that defines for_each for random access iterator

#include <bits/stl_bvector.h>
#include <bits/stl_pair.h>
#include <runtime/rtcmn.h>

template<class Iterator_t, class Functor_t>
void for_each_impl(Iterator_t begin, Iterator_t end, Functor_t && f, Entities_t & es, auto_partitioner * p)
{
    //use a divide-and-conquer method to do for_each
    size_t divide_times = static_cast<int>(log2(ff::rt::rt_concurrency()));
    uint64_t count = end - begin;
    es = std::make_shared<internal::paras_with_lock>();
    for_each_impl_auto_partition(begin, end, std::forward<Functor_t>(f), es, count, divide_times);
}

template<class Iterator_t, class Functor_t>
void for_each_impl_auto_partition(Iterator_t begin, Iterator_t end, Functor_t && f,
        Entities_t & es, size_t count, size_t divide_times)
{
    Iterator_t t = begin;
    Iterator_t bt = begin;
    size_t left = count;
    std::vector<para<void> > lgroup;
    while(divide_times !=0 && left != 1)
    {
        size_t sc = left /2;
        left = left - sc;
        size_t c = 0;
        bt = t;
        t = t + sc;
        para<void> p;
        p([bt, t, sc, &f, &es, divide_times](){
            for_each_impl_auto_partition(bt, t, std::move(f), es, sc, divide_times -1);});
        lgroup.push_back(p);
        /*
        if(left == 1) {
            m_oParasLock.lock();
            m_oParas.insert(std::make_pair(iterator_range(bt, t), p));
            m_oParasLock.unlock();
        }*/
        divide_times --;
    }

    es->lock.lock();
    for(int i = 0; i < lgroup.size(); ++i)
        es->entities.push_back(lgroup[i]);
    es->lock.unlock();
    while (t!=end)
    {
        f(t);
        t++;
    }
}

template<class Iterator_t, class Functor_t>
void for_each_impl(Iterator_t begin, Iterator_t end, Functor_t && f, Entities_t & es, simple_partitioner * p)
{
    ff::thrd_id_t this_id = ff::rt::get_thrd_id();
    size_t concurrency = ff::rt::rt_concurrency();//TODO(A.A) this may be optimal.
    //TODO(A.A) we may have another partition approach!
    uint64_t count = end-begin;
    Iterator_t t = begin;
    uint64_t step = count/ concurrency;
    uint64_t ls = count % concurrency;

    es = std::make_shared<internal::paras_with_lock >();


    uint16_t counter = 0;
    int32_t thrd_id = 0;
    while(step != 0 && t!=end && thrd_id < concurrency)
    {
        thrd_id ++;
        if(thrd_id == this_id) {
            continue;
        }
        Iterator_t tmp = std::min(static_cast<Iterator_t>(t + step), end);

        para<void> p;
        p([t, tmp, f]() {
            _DEBUG(LOG_INFO(para) <<"for_each generated task start running...")
            Iterator_t lt = t;
            while(lt != tmp)
            {
                _DEBUG(LOG_INFO(para) <<"for_each generated task run step f("<< ")")
                f(lt);
                lt ++;
            }
            _DEBUG(LOG_INFO(para) <<"for_each generated task run over!")
        }
#ifdef USING_MIMO_QUEUE
                  , thrd_id
#endif
        );
        es->lock.lock();
        m_pParas->insert(std::make_pair(iterator_range(t, tmp), p));
        es->entities.push_back(p);
        es->lock.unlock();
        t=tmp;
    }
    while(!m_oPlaceHolder.check_if_over()) ff::rt::yield();

    m_pParas->insert(std::make_pair(iterator_range(t, end), m_oPlaceHolder));
    while(t != end) {
        f(t);
        t++;
    }
    _DEBUG(LOG_INFO(para)<<"for_each generates "<<es->entities.size()<<" para<> tasks")
}
