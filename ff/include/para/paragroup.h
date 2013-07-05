#ifndef FF_PARA_PARA_GROUND_H_
#define FF_PARA_PARA_GROUND_H_
#include "common/common.h"
#include "para/para.h"
#include "para/para_helper.h"

namespace ff {

enum group_optimizer
{
  auto_partition,
  max_partition, 
  //other may be here
};
  
template<class RT>
class paragroup {
public:
  typedef RT ret_type;
public:
		
template<class PT, class WT>
class para_accepted_wait
{
    para_accepted_wait & operator = (const para_accepted_wait<PT, WT> &) = delete;
public:
    para_accepted_wait(const para_accepted_wait<PT, WT> &) = default;
    para_accepted_wait(PT & p, const WT & w)
        : m_refP(p)	
		, m_oWaiting(w){}

    template<class F>
    auto		operator ()(F && f) -> internal::para_accepted_call<PT, ret_type>
    {
		internal::para_impl_ptr<ret_type> pImpl = internal::make_para_impl<ret_type>(f);
		m_refP.m_pImpl = pImpl;
		internal::para_impl_wait_ptr<WT> pTask = std::make_shared<internal::para_impl_wait<WT> >(m_oWaiting, m_refP.m_pImpl);
		internal::schedule(pTask);
        return internal::para_accepted_call<PT, ret_type>(m_refP);
    }
protected:
    PT & m_refP;
	WT	m_oWaiting;
};//end class para_accepted_wait;

    void		push(const para<RT> & p) {}
    
    template <class WT>
    para_accepted_wait<paragroup<RT>,WT> operator[](const WT & cond)
    {
        return para_accepted_wait<paragroup<RT>,WT>(*this, cond);
    }
    
    
    template<class Iterator_t, class Functor_t>
    auto for_each(Iterator_t begin, Iterator_t end, Functor_t f) 
    -> internal::para_accepted_call<paragroup<RT>, RT>
    {
        Iterator_t t = begin;
        while(t!= end)
        {
            f(*t);
	    t++;
        }
        return internal::para_accepted_call<paragroup<RT>, RT>(*this);
    }
};//end class paragroup
}//end namespace ff

#endif
