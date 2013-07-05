#ifndef FF_PARA_PAEA_H_
#define FF_PARA_PAEA_H_
#include "common/common.h"
#include "common/function_traits.h"
#include "para/para_helper.h"
#include "para/exception.h"
#include "para/para_impl.h"
#include "runtime/rtcmn.h"
#include "common/log.h"


namespace ff {
	
using namespace ff::utils;
template<typename RT = void>
class para {
public:
    typedef RT  ret_type;
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

    para()
        : m_pImpl(nullptr) {};
    template<class WT>
    para_accepted_wait<para<RT>, WT> operator[](const WT & cond)
    {
        return para_accepted_wait<para<RT>, WT>(*this,cond);
    }
    template<class F>
    auto		exe(const F & f) -> internal::para_accepted_call<para<RT>, RT>
    {
        if(m_pImpl)
            throw used_para_exception();
        m_pImpl = internal::make_para_impl<ret_type>(f);
        internal::schedule(m_pImpl);
        LOG_INFO(para)<<"task "<<m_pImpl.get()<<" scheduled";
        return internal::para_accepted_call<para<RT>, RT>(*this);
    }
    template<class F>
    auto		operator ()(const F & f) -> internal::para_accepted_call<para<RT>, RT>
    {
        return exe(f);
    }

    auto get() -> typename std::enable_if< !std::is_void<RT>::value,RT>::type &
	{
        return m_pImpl->get();
    }

    exe_state	get_state()
    {
        if(m_pImpl)
            return m_pImpl->get_state();
        return exe_state::exe_unknown;
    }
    bool	check_if_over()
    {
        if(m_pImpl)
            return m_pImpl->check_if_over();
        return false;
    }

    internal::para_impl_ptr<RT> get_internal_impl() {
        return m_pImpl;
    }
protected:
    internal::para_impl_ptr<RT> m_pImpl;
};//end class para;

/*
template<>
class para<void> {
public:
    typedef void ret_type;
public:
    para()
        : m_pImpl() {};

    template <class WT>
    internal::para_accepted_wait<para<void>, WT> operator[](const WT & cond)
    {
        return internal::para_accepted_wait<para<void>, WT>(*this, cond);
    }
    template<class F>
    auto		exe(F && f) -> internal::para_accepted_call<para<void>, void>
    {
        internal::schedule(m_pImpl);
        LOG_INFO(para)<<"task(void) "<<m_pImpl.get()<<" scheduled";
        return internal::para_accepted_call<para<void>, void>(*this);
    }
    template<class F>
    auto		operator ()(F && f) -> internal::para_accepted_call<para<void>, void>
    {
        if(m_pImpl)
            m_pImpl = std::make_shared<internal::para_impl<void> >([&f]() {
            f();
        });
        throw used_para_exception();
        return exe(f);
    }

    exe_state	get_state()
    {
        if(m_pImpl)
            return m_pImpl->get_state();
        return exe_state::exe_unknown;
    }
    bool	check_if_over()
    {
        if(m_pImpl)
            return m_pImpl->check_if_over();
        return false;
    }

    internal::para_impl_ptr<void> get_internal_impl() {
        return m_pImpl;
    }
    void		set_internal_impl(internal::para_impl_ptr<RT> &pImpl)
	{
		m_pImpl = pImpl;
	}

protected:
    internal::para_impl_ptr<void> m_pImpl;
};*///end class para<void>

template<>
class para<void> {
public:
    typedef void  ret_type;
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

    para()
        : m_pImpl(nullptr) {};
    template<class WT>
    para_accepted_wait<para<ret_type>, WT> operator[](const WT & cond)
    {
        return para_accepted_wait<para<ret_type>, WT>(*this,cond);
    }
    template<class F>
    auto		exe(const F & f) -> internal::para_accepted_call<para<ret_type>, ret_type>
    {
        if(m_pImpl)
            throw used_para_exception();
        m_pImpl = internal::make_para_impl<ret_type>(f);
        internal::schedule(m_pImpl);
        LOG_INFO(para)<<"task "<<m_pImpl.get()<<" scheduled";
        return internal::para_accepted_call<para<ret_type>, ret_type>(*this);
    }
    template<class F>
    auto		operator ()(const F & f) -> internal::para_accepted_call<para<ret_type>, ret_type>
    {
        return exe(f);
    }

    exe_state	get_state()
    {
        if(m_pImpl)
            return m_pImpl->get_state();
        return exe_state::exe_unknown;
    }
    bool	check_if_over()
    {
        if(m_pImpl)
            return m_pImpl->check_if_over();
        return false;
    }

    internal::para_impl_ptr<ret_type> get_internal_impl() {
        return m_pImpl;
    }
protected:
    internal::para_impl_ptr<ret_type> m_pImpl;
};//end class para;


}//end namespace ff
#endif
