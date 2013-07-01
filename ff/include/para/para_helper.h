#ifndef FF_PARA_PARA_HELPER_H_
#define FF_PARA_PARA_HELPER_H_
#include "common/function_traits.h"
#include "runtime/rtcmn.h"

namespace ff {
template<class RT> class para;

namespace internal {
using namespace ff::utils;
template<class RT> class para_impl;
template <class T>
class para_ret {
public:
    para_ret(para_impl<T> & p)
        : m_refP(p)
        , m_oValue() {}

    T & get() {
        return m_oValue;
    }
    void set(T& v) {
        m_oValue = v;
    }
    void set(T&& v) {
        m_oValue = v;
    }

protected:
    para_impl<T> &	m_refP;
    T m_oValue;
};//end class para_ret;


template<class PT, class RT>
class para_accepted_call {
public:
    para_accepted_call(PT& p)
        : m_refP(p) {}

    template<class FT>
    //void  then(FT && f, typename std::enable_if<std::is_void<typename function_res_traits<FT>::ret_type>::value, bool>::type * p = nullptr)
    auto  then(FT && f)
    -> typename std::enable_if<std::is_void<typename function_res_traits<FT>::ret_type>::value, void>::type
    {
		if(!m_refP.check_if_over())
			::ff::rt::yield_and_ret_until([m_refP](){return m_refP.check_if_over();});
        f(m_refP.get());
    }

    template<class FT>
    auto  then(FT && f ) ->
    typename std::remove_reference<typename function_res_traits<FT>::ret_type>::type &&
    {
		if(!m_refP.check_if_over())
			::ff::rt::yield_and_ret_until([m_refP](){return m_refP.check_if_over();});
        return f(m_refP.get());
    }

protected:
    PT & m_refP;
};//end class para_accepted_call

template<class PT>
class para_accepted_call<PT, void> {
public:
    para_accepted_call(PT& p)
        : m_refP(p) {}

    template<class FT>
    //void  then(FT && f, typename std::enable_if<std::is_void<typename function_res_traits<FT>::ret_type>::value, bool>::type * p = nullptr)
    auto  then(FT && f)
    -> typename std::enable_if<std::is_void<typename function_res_traits<FT>::ret_type>::value, void>::type
    {
		if(!m_refP.check_if_over())
			::ff::rt::yield_and_ret_until([m_refP](){return m_refP.check_if_over();});
        f();
    }

    template<class FT>
    auto  then(FT && f ) ->
    typename std::enable_if<
	      !std::is_void<typename function_res_traits<FT>::ret_type>::value,
	    typename std::remove_reference<typename function_res_traits<FT>::ret_type>::type
	    >::type 
    {
		if(!m_refP.check_if_over())
			::ff::rt::yield_and_ret_until([m_refP](){return m_refP.check_if_over();});
        return f();
    }

protected:
    PT & m_refP;
};//end class para_accepted_call

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
    auto		operator ()(F && f) -> para_accepted_call<PT, typename PT::ret_type>
    {
		auto pTask = std::make_shared<para_impl_wait<WT>>(m_oWaiting, m_refP);
		::ff::rt::schedule(pTask);
        return para_accepted_call<PT, typename PT::ret_type>(m_refP);
    }

protected:
    PT & m_refP;
	WT	m_oWaiting;
};//end class para_accepted_wait;


}//end namespace internal;
}//end namespace ff
#endif
