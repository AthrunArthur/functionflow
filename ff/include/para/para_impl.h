#ifndef FF_PARA_PARA_IMPL_H_
#define FF_PARA_PARA_IMPL_H_

#include "common/common.h"
//#include "runtime/taskbase.h"
#include "para/para_helper.h"

namespace ff {
namespace internal
{
template<class RT>
class para_impl //: public ff::rt::task_base
{
public:
    para_impl(const std::function<RT ()> & f)
	//: ff::rt::task_base(TKind::user_t)
	: m_oRet(*this)
	, m_oFunc(f)
	, m_iES(exe_state::exe_unknown){}
	
	virtual void	run()
	{
		m_oRet.set(m_oFunc());
	}
    RT & get() {
        return m_oRet.get();
    }
    
    
    exe_state	get_state()
	{
		return m_iES;
	}
	bool	check_if_over()
	{
		if(m_iES == exe_state::exe_over)
			return true;
		return false;
	}
	
protected:
    internal::para_ret<RT>	m_oRet;
	std::function<RT ()> m_oFunc;
	exe_state  m_iES;
};//end class para_impl

template<>
class para_impl<void> //: public ff::rt::task_base
{
public:
    para_impl(std::function<void ()> f)
	//: ff::rt::task_base(TKind::user_t)
	: m_iES(exe_state::exe_unknown)
	, m_oFunc(f){}
	
	virtual void	run()
	{
		m_oFunc();
	}
    
    
    exe_state	get_state()
	{
		return m_iES;
	}
	bool	check_if_over()
	{
		if(m_iES == exe_state::exe_over)
			return true;
		return false;
	}
	
protected:
	exe_state  m_iES;
	std::function<void ()> m_oFunc;
};//end class para_impl_ptr

template<class RT>
using para_impl_ptr = std::shared_ptr<para_impl<RT>>;

}//end namespace internal
}//end namespace ff
#endif