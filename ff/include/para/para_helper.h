#ifndef FF_PARA_PARA_HELPER_H_
#define FF_PARA_PARA_HELPER_H_
#include "common/common.h"
#include "common/function_traits.h"
#include "runtime/rtcmn.h"
#include "para/para_impl.h"
#include "common/log.h"

namespace ff {
template<class RT> class para;

namespace internal {
using namespace ff::utils;
template<class RT> class para_impl;


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
			::ff::rt::yield_and_ret_until([this](){return m_refP.check_if_over();});
        f(m_refP.get());
    }

    template<class FT>
    auto  then(FT && f ) ->
    typename std::remove_reference<typename function_res_traits<FT>::ret_type>::type &&
    {
		if(!m_refP.check_if_over())
			::ff::rt::yield_and_ret_until([this](){return m_refP.check_if_over();});
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
			::ff::rt::yield_and_ret_until([this](){return m_refP.check_if_over();});
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
			::ff::rt::yield_and_ret_until([this](){return m_refP.check_if_over();});
        return f();
    }

protected:
    PT & m_refP;
};//end class para_accepted_call



}//end namespace internal;
}//end namespace ff
#endif
