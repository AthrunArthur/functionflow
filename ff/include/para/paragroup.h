#ifndef FF_PARA_PARA_GROUND_H_
#define FF_PARA_PARA_GROUND_H_
#include "common/common.h"
#include "para/para.h"
#include "para/para_helper.h"

namespace ff {

	namespace internal{
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
		, m_oWaiting(w){}

	template<class Iterator_t, class Functor_t>
    auto for_each(Iterator_t begin, Iterator_t end, Functor_t && f) 
    -> internal::para_accepted_call<paragroup, void>
    {
		int concurrency = std::thread::hardware_concurrency();//TODO(A.A) this may be optimal.
		//TODO(A.A) we may have another partition approach!
		uint64_t count = 0;
        Iterator_t t = begin;
        while(t!= end)
        {
			t++;
			count ++;
        }
        uint64_t step = count/ concurrency;
		if(!(count %concurrency))
			step ++;
        
		t = begin;
		
		while(t!=end)
		{
			Iterator_t tmp = t;
			count = 0;
			while(tmp != end && count<step)
			{
				tmp ++;
				count ++;
			}
			para<void> p;
			p[m_oWaiting]([t, tmp, &f](){
				Iterator_t lt = t;
				while(lt != tmp)
				{
					f(*lt);
					lt ++;
				}
			});
			m_refP.m_oEntities.push_back(std::move(p));
			
			t=tmp;
		}
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
    
    
    template<class Iterator_t, class Functor_t>
    auto for_each(Iterator_t begin, Iterator_t end, Functor_t && f) 
    -> internal::para_accepted_call<paragroup, void>
    {
		int concurrency = std::thread::hardware_concurrency();//TODO(A.A) this may be optimal.
		//TODO(A.A) we may have another partition approach!
		uint64_t count = 0;
        Iterator_t t = begin;
        while(t!= end)
        {
			t++;
			count ++;
        }
        uint64_t step = count/ concurrency;
		if(!(count %concurrency))
			step ++;
        
		t = begin;
		
		while(t!=end)
		{
			Iterator_t tmp = t;
			count = 0;
			while(tmp != end && count<step)
			{
				tmp ++;
				count ++;
			}
			para<void> p;
			p([t, tmp, &f](){
				Iterator_t lt = t;
				while(lt != tmp)
				{
					f(*lt);
					lt ++;
				}
			});
			m_oEntities.push_back(std::move(p));
			
			t=tmp;
		}
        return internal::para_accepted_call<paragroup, ret_type>(*this);
    }
    
protected:
	friend internal::wait_all all(paragroup & pg);
	friend internal::wait_any any(paragroup & pg);
    std::vector<para<void> > & all_entities(){return m_oEntities;};
    
protected:
	std::vector<para<void> >	m_oEntities;
};//end class paragroup



}//end namespace ff

#endif
