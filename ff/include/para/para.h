#ifndef FF_PARA_PAEA_H_
#define FF_PARA_PAEA_H_
#include "common/common.h"
#include "common/function_traits.h"
#include "para/para_helper.h"
#include "para/exception.h"

namespace ff {
using namespace ff::utils;
template<typename RT = void>
class para {
public:
    typedef RT  ret_type;
public:
    para(const para<RT> &) = delete;
    para<RT> & operator =(const para<RT> &) = delete;
    para()
        : m_oRet(*this)
        , m_bIsUsed(false) {};

    template <class WT>
    internal::para_accepted_wait<para<RT>> operator[](const WT & cond)
    {
        return internal::para_accepted_wait<para<RT>>(*this);
    }
    template<class F>
    auto		exe(F && f) -> internal::para_accepted_call<para<RT>, RT>
    {
        if(m_bIsUsed)
            throw used_para_exception();
        m_oRet.set(f());
        m_bIsUsed = true;
        return internal::para_accepted_call<para<RT>, RT>(*this);
    }
    template<class F>
    auto		operator ()(F && f) -> internal::para_accepted_call<para<RT>, RT>
    {
        return exe(f);
    }

    RT & get() {
        return m_oRet.get();
    }

protected:
    internal::para_ret<RT>	m_oRet;
    bool		m_bIsUsed;
};//end class para;


template<>
class para<void> {
public:
    typedef void ret_type;
public:
    para(const para<void> &) = delete;
    para<void> & operator =(const para<void> &) = delete;
    para()
        : m_bIsUsed(false) {};

    template <class WT>
    internal::para_accepted_wait<para<void>> operator[](const WT & cond)
    {
        if(m_bIsUsed)
            throw used_para_exception();
        return internal::para_accepted_wait<para<void>>(*this);
    }
    template<class F>
    auto		exe(F && f) -> internal::para_accepted_call<para<void>, void>
    {
        f();
        m_bIsUsed = true;
        return internal::para_accepted_call<para<void>, void>(*this);
    }
    template<class F>
    auto		operator ()(F && f) -> internal::para_accepted_call<para<void>, void>
    {
        if(m_bIsUsed)
            throw used_para_exception();
        return exe(f);
    }
protected:
    bool	m_bIsUsed;
};//end class para<void>

}//end namespace ff
#endif
