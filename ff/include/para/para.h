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
#include "para/para_accepted_wait.h"
    para()
        : m_pImpl(nullptr) {};

    ~para()
    {
    }
    template<class WT>
    para_accepted_wait<para<RT>, WT> operator[](WT && cond)
    {
        return para_accepted_wait<para<RT>, WT>(*this,std::forward<WT>(cond));
    }
    template<class F>
    auto		exe(F && f) -> internal::para_accepted_call<para<RT>, RT>
    {
        if(m_pImpl)
            throw used_para_exception();
        m_pImpl = internal::make_para_impl<ret_type>(std::forward<F>(f));
        internal::schedule(m_pImpl);
        return internal::para_accepted_call<para<RT>, RT>(*this);
    }
    template<class F>
    auto		operator ()(F && f) -> internal::para_accepted_call<para<RT>, RT>
    {
        return exe(std::forward<F>(f));
    }
#if USING_MIMO_QUEUE
    template<class F>
    auto		exe(F && f, int32_t thrd_id) -> internal::para_accepted_call<para<RT>, RT>
    {
        if(m_pImpl)
            throw used_para_exception();
        m_pImpl = internal::make_para_impl<ret_type>(std::forward<F>(f));
        internal::schedule(m_pImpl, thrd_id);
        return internal::para_accepted_call<para<RT>, RT>(*this);
    }
    template<class F>
    auto		operator ()(F && f, int32_t thrd_id) -> internal::para_accepted_call<para<RT>, RT>
    {
        return exe(std::forward<F>(f), thrd_id);
    }
#endif
    auto get() -> typename std::enable_if< !std::is_void<RT>::value,RT>::type &
    {
        return m_pImpl->get();
    }

    exe_state	get_state()
    {
        if(m_pImpl)
            return m_pImpl->get_state();
        return exe_state::exe_over;
    }
    bool	check_if_over()
    {
        if(m_pImpl)
            return m_pImpl->check_if_over();
        return true;
    }

    internal::para_impl_ptr<RT> get_internal_impl() {
        return m_pImpl;
    }
protected:
    internal::para_impl_ptr<RT> m_pImpl;
};//end class para;

template<>
class para<void> {
public:
    typedef void  ret_type;
public:
#include "para/para_accepted_wait.h"
    para()
        : m_pImpl(nullptr) {};
    ~para()
    {
    }
    template<class WT>
    para_accepted_wait<para<ret_type>, WT> operator[](WT && cond)
    {
        return para_accepted_wait<para<ret_type>, WT>(*this,std::forward<WT>(cond));
    }
    template<class F>
    auto		exe(F && f) -> internal::para_accepted_call<para<ret_type>, ret_type>
    {
        if(m_pImpl)
            throw used_para_exception();
        m_pImpl = internal::make_para_impl<ret_type>(std::forward<F>(f));
        internal::schedule(m_pImpl);
        return internal::para_accepted_call<para<ret_type>, ret_type>(*this);
    }
    template<class F>
    auto		operator ()(F && f) -> internal::para_accepted_call<para<ret_type>, ret_type>
    {
        return exe(std::forward<F>(f));
    }
#ifdef USING_MIMO_QUEUE
    template<class F>
    auto		exe(F && f, int32_t thrd_id) -> internal::para_accepted_call<para<ret_type>, ret_type>
    {
        if(m_pImpl)
            throw used_para_exception();
        m_pImpl = internal::make_para_impl<ret_type>(std::forward<F>(f));
        internal::schedule(m_pImpl, thrd_id);
        return internal::para_accepted_call<para<ret_type>, ret_type>(*this);
    }
    template<class F>
    auto		operator ()(F && f, int32_t thrd_id) -> internal::para_accepted_call<para<ret_type>, ret_type>
    {
        return exe(std::forward<F>(f), thrd_id);
    }
#endif
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
