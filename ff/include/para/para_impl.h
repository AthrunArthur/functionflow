#ifndef FF_PARA_PARA_IMPL_H_
#define FF_PARA_PARA_IMPL_H_

#include "common/common.h"
#include "runtime/taskbase.h"
#include "para/para_helper.h"

namespace ff {
namespace internal
{
template<class RT>
class para_impl : public ff::rt::task_base
{
public:
    para_impl(const std::function<RT ()> & f)
	: ff::rt::task_base(TKind::user_t)
	, m_oRet(*this)
	, m_oFunc(f)
	, m_iES(exe_state::exe_unknown){}
	
	virtual void	run()
	{
		m_oRet.set(m_oFunc());
	}
    RT & get() {
        return m_oRet.get();
		m_iES.store(exe_state::exe_over);
    }
    
    
    exe_state	get_state()
	{
		return m_iES.load();
	}
	bool	check_if_over()
	{
		if(m_iES.load() == exe_state::exe_over)
			return true;
		return false;
	}
	
protected:
    internal::para_ret<RT>	m_oRet;
	std::function<RT ()> m_oFunc;
	std::atomic<exe_state>  m_iES;
};//end class para_impl

template<>
class para_impl<void> : public ff::rt::task_base
{
public:
    para_impl(std::function<void ()> f)
	: ff::rt::task_base(TKind::user_t)
	, m_iES(exe_state::exe_unknown)
	, m_oFunc(f){}
	
	virtual void	run()
	{
		m_oFunc();
		m_iES.store(exe_state::exe_over);
	}
    
    
    exe_state	get_state()
	{
		return m_iES.load();
	}
	bool	check_if_over()
	{
		if(m_iES.load() == exe_state::exe_over)
			return true;
		return false;
	}
	
protected:
	std::atomic<exe_state>  m_iES;
	std::function<void ()> m_oFunc;
};//end class para_impl_ptr
template<class RT>
using para_impl_ptr = std::shared_ptr<para_impl<RT>>;


template<class WT>
class para_impl_wait : public ff::rt::task_base
{
public:
	para_impl_wait(WT & w, para_impl_ptr & p)
	:m_iES(exe_state::exe_unknown)
	, m_pFunc(p)
	, m_oWaitingPT(w){}
	
	virtual void run()
	{
		if(m_oWaitingPT.get_state() != exe_state::exe_over)
		{
			::ff::rt::yield_and_ret_until([&m_oWaitingPT](){return m_oWaitingPT.check_if_over();});
		}
		m_pFunc->run();
		m_iES.store(exe_state::exe_over);
	}
	
	exe_state	get_state()
	{
		return m_iES.load();
	}
	bool	check_if_over()
	{
		if(m_iES.load() == exe_state::exe_over)
			return true;
		return false;
	}
protected:
	std::atomic<exe_state> m_iES;
	para_impl_ptr 	m_pFunc;
	WT 	m_oWaitingPT;
};//end class para_impl_wait;


}//end namespace internal
}//end namespace ff
#endif