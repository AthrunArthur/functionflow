#ifndef FF_PARA_PARA_GROUND_H_
#define FF_PARA_PARA_GROUND_H_
#include "common/common.h"
#include "para/para.h"
#include "para/para_helper.h"
#include "common/log.h"
#include "runtime/env.h"


namespace ff {

namespace internal {
class wait_all;
class wait_any;
}//end namespace internal
enum group_optimizer
{
    auto_partition,
    max_partition,
    //other may be here
};

class paragroup {
public:
    typedef void ret_type;
public:
    template<class PT, class WT>
    class para_accepted_wait
    {
        para_accepted_wait & operator = (const para_accepted_wait<PT, WT> &) = delete;
    public:
        typedef typename PT::ret_type ret_type;
        para_accepted_wait(const para_accepted_wait<PT, WT> &) = default;
        para_accepted_wait(PT & p, const WT & w)
            : m_refP(p)
            , m_oWaiting(w) {}


        template<class Iterator_t, class Functor_t>
        auto for_each(Iterator_t begin, Iterator_t end, Functor_t && f)
        -> typename std::enable_if<
        std::is_arithmetic<
        typename std::remove_cv<Iterator_t>::type>::value,
                 internal::para_accepted_call<paragroup, void>>::type
        {
            for_each_impl(begin, end, std::forward<Functor_t>(f), m_refP.m_pEntities);
            return internal::para_accepted_call<paragroup, ret_type>(m_refP);
        }

        template<class Iterator_t, class Functor_t>
        auto for_each(Iterator_t begin, Iterator_t end, Functor_t && f)
        -> typename std::enable_if<
        !std::is_arithmetic<typename std::remove_cv<Iterator_t>::type>::value,
        internal::para_accepted_call<paragroup, void>>::type
        {
            for_each_impl(begin, end, [f](const Iterator_t & t) {
                f(*t);
            }, m_refP.m_pEntities);
            return internal::para_accepted_call<paragroup, ret_type>(m_refP);
        }


    protected:
        PT & m_refP;
        WT	m_oWaiting;
    };//end class para_accepted_wait;

    //void		push(const para<RT> & p) {}

    template <class WT>
    para_accepted_wait<paragroup,WT> operator[](WT && cond)
    {
        return para_accepted_wait<paragroup,WT>(*this, std::forward<WT>(cond));
    }

    para<void> &  operator [](int index)
    {
        _DEBUG(
            if(!m_pEntities)
    {
        LOG_FATAL(para)<<" m_pEntities is null";
        }
        )
        return (*m_pEntities)[index];
    }
    size_t 	size() const
    {
        if(m_pEntities)
            return m_pEntities->size();
        return 0;
    }
    ~paragroup()
    {
    }

    template<class Iterator_t, class Functor_t>
    auto for_each(Iterator_t begin, Iterator_t end, Functor_t && f)
    -> typename std::enable_if<
    std::is_arithmetic<
    typename std::remove_cv<Iterator_t>::type>::value,
             internal::para_accepted_call<paragroup, void>>::type
    {
        for_each_impl(begin, end, std::forward<Functor_t>(f), m_pEntities);
        return internal::para_accepted_call<paragroup, ret_type>(*this);
    }

    template<class Iterator_t, class Functor_t>
    auto for_each(Iterator_t begin, Iterator_t end, Functor_t && f)
    -> typename std::enable_if<
    !std::is_arithmetic<typename std::remove_cv<Iterator_t>::type>::value,
    internal::para_accepted_call<paragroup, void>>::type
    {
        for_each_impl(begin, end, [f](const Iterator_t & t) {
            f(*t);
        }, m_pEntities);
        return internal::para_accepted_call<paragroup, ret_type>(*this);
    }

    void add(const para< void >&  p)
    {
        if(!m_pEntities)
            m_pEntities = std::make_shared<std::vector<para<void> > >();
        m_pEntities->push_back(p);
    }

    void clear()
    {
        m_pEntities.reset();
    }
protected:
    typedef std::shared_ptr<std::vector<para<void> > > Entities_t;
    template<class Iterator_t, class Functor_t>
    static void for_each_impl(Iterator_t begin, Iterator_t end, Functor_t && f, Entities_t & es)
    {
      thread_local  static ff::rt::thrd_id_t this_id = ff::rt::get_thrd_id();
        int concurrency = ff::rt::rt_concurrency();//TODO(A.A) this may be optimal.
        //TODO(A.A) we may have another partition approach!
        uint64_t count = 0;
        Iterator_t t = begin;
        while(t!= end)
        {
            t++;
            count ++;
        }
        uint64_t step = count/ concurrency;
        uint64_t ls = count % concurrency;

        t = begin;
        es = std::make_shared<std::vector<para<void> > >();


        uint16_t counter = 0;//added
        int32_t thrd_id = 0;
        while(t!=end && thrd_id < concurrency)
        {
	  if(thrd_id == this_id){
	    thrd_id ++;
	    continue;
	  }
            Iterator_t tmp = t;
            count = 0;
            uint64_t upperbound = step;//added
            if(counter < ls)
                upperbound ++;
            counter ++;
            while(tmp != end && count<upperbound)
            {
                tmp ++;
                count ++;
            }
            para<void> p;
            p([t, tmp, f]() {
                _DEBUG(LOG_INFO(para) <<"for_each generated task start running...")
                Iterator_t lt = t;
                while(lt != tmp)
                {
                    _DEBUG(LOG_INFO(para) <<"for_each generated task run step f("<< lt<<")")
                    f(lt);
                    lt ++;
                }
                _DEBUG(LOG_INFO(para) <<"for_each generated task run over!")
            }
#ifdef USING_MIMO_QUEUE
            , thrd_id
#endif
             );
            es->push_back(p);
            thrd_id ++;
            t=tmp;
        }
        while(t != end){f(t); t++;}
        _DEBUG(LOG_INFO(para)<<"for_each generates "<<es->size()<<" para<> tasks")
    }
protected:
    friend internal::wait_all all(paragroup & pg);
    friend internal::wait_any any(paragroup & pg);
    std::shared_ptr<std::vector<para<void> > > & all_entities() {
        return m_pEntities;
    };

protected:
    std::shared_ptr<std::vector<para<void> > >	m_pEntities;
};//end class paragroup



}//end namespace ff

#endif
