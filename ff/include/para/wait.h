/***********************************************
The MIT License (MIT)

Copyright (c) 2012 Athrun Arthur <athrunarthur@gmail.com>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*************************************************/
#ifndef FF_PARA_WAIT_H_
#define FF_PARA_WAIT_H_
#include "common/common.h"
#include "common/tuple_type.h"
#include "para/para.h"
#include "para/paragroup.h"
#include "para/paracontainer.h"
#include "para/bin_wait_func_deducer.h"
#include "common/log.h"

namespace ff {
template<class RT>
class para;
namespace internal
{
template <class T1, class T2>
class wait_and
{
public:
    typedef typename std::remove_reference<T1>::type T1_t;
    typedef typename std::remove_reference<T2>::type T2_t;
    typedef bin_wait_func_deducer<typename T1_t::ret_type, typename T2_t::ret_type> deduct_t;
    typedef typename deduct_t::and_type ret_type;
public:
    wait_and(T1 && t1, T2 && t2)
        : m_1(t1)
        , m_2(t2)
        , m_iES(exe_state::exe_unknown) {}
    /*
     wait_and(T1_t & t1, T2_t & t2)
    : m_1(t1)
    , m_2(t2)
    , m_iES(exe_state::exe_unknown){}
    */
    template<class FT>
    auto  then(FT && f)
    -> typename std::enable_if<
    std::is_void<typename function_res_traits<FT>::ret_type>::value &&
    !utils::function_args_traits<FT>::is_no_args, void>::type
    {
        if(!check_if_over())
            ::ff::rt::yield_and_ret_until([this]() {
            return check_if_over();
        });
        deduct_t::void_func_and(std::forward<FT>(f), m_1, m_2);
    }

    template<class FT>
    auto  then(FT && f ) ->
    typename std::enable_if<
    !std::is_void<typename function_res_traits<FT>::ret_type>::value &&
    !utils::function_args_traits<FT>::is_no_args,
    typename std::remove_reference<typename function_res_traits<FT>::ret_type>::type
    >::type
    {
        if(!check_if_over())
            ::ff::rt::yield_and_ret_until([this]() {
            return check_if_over();
        });
        return deduct_t::ret_func_and(std::forward<FT>(f), m_1, m_2);
    }

    template<class FT>
    auto  then(FT && f)
    -> typename std::enable_if<std::is_void<typename function_res_traits<FT>::ret_type>::value &&
    utils::function_args_traits<FT>::is_no_args, void>::type
    {
        if(!check_if_over())
            ::ff::rt::yield_and_ret_until([this]() {
            return check_if_over();
        });
        f();
    }

    template<class FT>
    auto  then(FT && f ) ->
    typename std::enable_if<
    !std::is_void<typename function_res_traits<FT>::ret_type>::value &&
    utils::is_no_args_function<FT>::value,
          typename std::remove_reference<typename function_res_traits<FT>::ret_type>::type
          >::type
    {
        if(!check_if_over())
            ::ff::rt::yield_and_ret_until([this]() {
            return check_if_over();
        });
        return f();
    }

    auto get() -> typename deduct_t::and_type
    {
        return deduct_t::wrap_ret_for_and(m_1, m_2);
    }

    exe_state	get_state()
    {
        if(m_iES != exe_state::exe_over)
            m_iES = (m_1.get_state() && m_2.get_state());
        return m_iES;
    }
    bool	check_if_over()
    {
        if(m_iES == exe_state::exe_over)
            return true;
        m_iES = ( m_1.get_state() && m_2.get_state());
        if(m_iES == exe_state::exe_over)
            return true;
        return false;
    }
protected:
    T1_t&  m_1;
    T2_t&  m_2;
    exe_state	m_iES;
};//end class wait_and


template <class T1, class T2>
class wait_or
{
public:
    typedef typename std::remove_reference<T1>::type T1_t;
    typedef typename std::remove_reference<T2>::type T2_t;
    typedef bin_wait_func_deducer<typename T1_t::ret_type, typename T2_t::ret_type> deduct_t;
    typedef typename deduct_t::or_type ret_type;
public:
    wait_or(T1 && t1, T2 && t2)
        : m_1(t1)
        , m_2(t2)
        , m_iES(exe_state::exe_unknown) {}
    /*
    wait_or(T1_t & t1, T2_t & t2)
        : m_1(t1)
        , m_2(t2)
    	, m_iES(exe_state::exe_unknown){
    }
    */
    template<class FT>
    auto  then(FT && f)
    -> typename std::enable_if<
    std::is_void<typename function_res_traits<FT>::ret_type>::value &&
    !utils::function_args_traits<FT>::is_no_args
    , void>::type
    {
        if(!check_if_over())
            ::ff::rt::yield_and_ret_until([this]() {
            return check_if_over();
        });
        deduct_t::void_func_or(std::forward<FT>(f), m_1, m_2);
    }

    template<class FT>
    auto  then(FT && f ) ->
    typename std::enable_if<
    !std::is_void<typename function_res_traits<FT>::ret_type>::value &&
    !utils::function_args_traits<FT>::is_no_args,
    typename std::remove_reference<typename function_res_traits<FT>::ret_type>::type
    >::type
    {
        if(!check_if_over())
            ::ff::rt::yield_and_ret_until([this]() {
            return check_if_over();
        });
        return deduct_t::ret_func_or(std::forward<FT>(f), m_1, m_2);
    }

    template<class FT>
    auto  then(FT && f)
    -> typename std::enable_if<std::is_void<typename function_res_traits<FT>::ret_type>::value &&
    utils::function_args_traits<FT>::is_no_args, void>::type
    {
        if(!check_if_over())
            ::ff::rt::yield_and_ret_until([this]() {
            return check_if_over();
        });
        f();
    }

    template<class FT>
    auto  then(FT && f ) ->
    typename std::enable_if<
    !std::is_void<typename function_res_traits<FT>::ret_type>::value &&
    utils::function_args_traits<FT>::is_no_args,
          typename std::remove_reference<typename function_res_traits<FT>::ret_type>::type
          >::type
    {
        if(!check_if_over())
            ::ff::rt::yield_and_ret_until([this]() {
            return check_if_over();
        });
        return f();
    }

    auto get() -> typename deduct_t::or_type
    {
        return deduct_t::wrap_ret_for_or(m_1, m_2);
    }

    exe_state	get_state()
    {
        if(m_iES != exe_state::exe_over)
            m_iES = ( m_1.get_state() || m_2.get_state() );
        return m_iES;
    }
    bool	check_if_over()
    {
        if(m_iES == exe_state::exe_over)
            return true;
        m_iES = ( m_1.get_state() || m_2.get_state() );
        if(m_iES == exe_state::exe_over)
            return true;
        return false;
    }
protected:
    T1_t &  m_1;
    T2_t &  m_2;
    exe_state	m_iES;
};//end class wait_or


class wait_all
{
public:
    typedef void ret_type;
    wait_all(std::shared_ptr<internal::paras_with_lock >  ps);


    template<class FT>
    auto  then(FT && f ) -> void
    {
        if(!check_if_over())
            ::ff::rt::yield_and_ret_until([this]() {
            return check_if_over();
        });
        f();
    }


    exe_state	get_state();
    bool	check_if_over();

protected:
    std::shared_ptr<internal::paras_with_lock >  all_ps;
    exe_state	m_iES;
};//end class wait_all

class wait_any
{
public:
    typedef void ret_type;
    wait_any(std::shared_ptr<internal::paras_with_lock >  ps);


    template<class FT>
    auto  then(FT && f ) -> void
    {
        if(!check_if_over())
            ::ff::rt::yield_and_ret_until([this]() {
            return check_if_over();
        });
        f();
    }


    exe_state	get_state();
    bool	check_if_over();

protected:
    std::shared_ptr<internal::paras_with_lock >  all_ps;
    exe_state	m_iES;
};//end class wait_any


}//end namespace internal

template<class T>
struct is_para_or_wait: public std::false_type {};
template<class T>
struct is_para_or_wait<para <T> > : public std::true_type {};
template<class T1, class T2>
struct is_para_or_wait<internal::wait_and<T1, T2> > : public std::true_type {};
template<class T1, class T2>
struct is_para_or_wait<internal::wait_or<T1, T2> > : public std::true_type {};
template<>
struct is_para_or_wait<internal::wait_all> : public std::true_type {};
template<>
struct is_para_or_wait<internal::wait_any> : public std::true_type {};

template<class T1, class T2>
auto operator &&(T1 && t1, T2 && t2)
->typename std::enable_if< is_para_or_wait<typename std::remove_reference<T1>::type>::value &&
is_para_or_wait<typename std::remove_reference<T2>::type>::value,
                internal::wait_and<T1, T2> >::type
{
    return internal::wait_and<T1, T2>(std::forward<T1>(t1), std::forward<T2>(t2));
}

template<class T1, class T2>
auto operator ||(T1 && t1, T2 && t2)
->typename std::enable_if< is_para_or_wait<typename std::remove_reference<T1>::type>::value &&
is_para_or_wait<typename std::remove_reference<T2>::type>::value,
                internal::wait_or<T1, T2> >::type
{
    return internal::wait_or<T1, T2>(std::forward<T1>(t1), std::forward<T2>(t2));
}

auto all(paragroup & pg) -> internal::wait_all;
auto any(paragroup & pg) -> internal::wait_any;

auto all(paracontainer & pc) -> internal::wait_all;
auto any(paracontainer & pc) -> internal::wait_any;

}//end namespace ff

#endif
