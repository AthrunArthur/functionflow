#ifndef FF_PARA_PAEA_H_
#define FF_PARA_PAEA_H_
#include "common/common.h"
#include "para/para_helper.h"
#include "common/function_traits.h"
#include <iostream>
namespace ff {
using namespace ff::utils;
template<typename RT = void>
class para {
public:
	typedef RT  ret_type;
public:
    class para_accepted_call {
    public:
        para_accepted_call(para<RT>& p)
            : m_refP(p){}

        template<class FT>
        void  then(FT && f, typename std::enable_if<std::is_void<typename function_res_traits<FT>::ret_type>::value, bool>::type * p = nullptr)
        {
            f(m_refP.get());
        }
        
        template<class FT>
        auto  then(FT && f ) ->  
        typename std::remove_reference<typename function_res_traits<FT>::ret_type>::type &&
        {
            return f(m_refP.get());
        }
        
    protected:
        para<RT> & m_refP;
    };//end class para_accepted_call

    class para_accepted_wait
    {
        para_accepted_wait & operator = (const para_accepted_wait &) = delete;
    public:
		para_accepted_wait(const para_accepted_wait &) = default;
        para_accepted_wait(para<RT> & p)
            : m_refP(p)	{}

        template<class F>
        auto		operator ()(F && f) -> para_accepted_call
        {
            return m_refP.exe(f);
        }

    protected:
        para<RT> & m_refP;
    };//end class para_accepted_wait;


public:
    para(const para<RT> &) = delete;
    para<RT> & operator =(const para<RT> &) = delete;
    para() 
	: m_oRet(*this){};

    template <class WT>
    para_accepted_wait operator[](const WT & cond)
    {
        return para_accepted_wait(*this);
    }
    template<class F>
    auto		exe(F && f) -> para_accepted_call
    {
        m_oRet.set(f());
		std::cout<<"exe: "<<m_oRet.get()<<std::endl;
        return para_accepted_call(*this);
    }
    template<class F>
    auto		operator ()(F && f) -> para_accepted_call
    {
        return exe(f);
    }

    RT & get() {
        return m_oRet.get();
    }

protected:
    internal::para_ret<RT>	m_oRet;
};//end class para;


template<>
class para<void> {
public:
	typedef void ret_type;
public:
    class para_accepted_call {
    public:
        para_accepted_call(para<void>& p)
            : m_refP(p){}

        template<class FT>
        void  then(FT && f, typename std::enable_if<std::is_void<typename function_res_traits<FT>::ret_type>::value, bool>::type * p = nullptr)
        {
            f();
        }
        
        template<class FT>
        auto  then(FT && f ) ->  
        typename std::remove_reference<typename function_res_traits<FT>::ret_type>::type &&
        {
            return f();
        }
        
    protected:
        para<void> & m_refP;
    };//end class para_accepted_call

    class para_accepted_wait
    {
        para_accepted_wait(const para_accepted_wait &) = delete;
        para_accepted_wait & operator = (const para_accepted_wait &) = delete;
    public:
        para_accepted_wait(para<void> & p)
            : m_refP(p)	{}

        template<class F>
        auto		operator ()(F && f) -> para_accepted_call &&
        {
            return m_refP.exe(f);
        }

    protected:
        para<void> & m_refP;
    };//end class para_accepted_wait;


public:
    para(const para<void> &) = delete;
    para<void> & operator =(const para<void> &) = delete;
    para() {};

    template <class WT>
    para_accepted_wait && operator[](const WT & cond)
    {
        return para_accepted_wait(*this);
    }
    template<class F>
    auto		exe(F && f) -> para_accepted_call &&
    {   
		f();
        return para_accepted_call(*this);
    }
    template<class F>
    auto		operator ()(F && f) -> para_accepted_call &&
    {
        return exe(f);
    }
};//end class para<void>

}//end namespace ff
#endif
