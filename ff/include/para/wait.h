#ifndef FF_PARA_WAIT_H_
#define FF_PARA_WAIT_H_
#include "common/common.h"
#include "common/tuple_type.h"
#include "para/para.h"
#include "para/paragroup.h"
#include "para/bin_wait_func_deducer.h"

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
		, m_iES(exe_state::exe_unknown){}

    template<class FT>
    auto  then(FT && f)
    -> typename std::enable_if<std::is_void<typename function_res_traits<FT>::ret_type>::value, void>::type
    {
		if(!check_if_over())
			::ff::rt::yield_and_ret_until([this](){return check_if_over();});
        deduct_t::void_func_and(std::forward<FT>(f), m_1, m_2);
    }

    template<class FT>
    auto  then(FT && f ) ->
    typename std::enable_if<
	      !std::is_void<typename function_res_traits<FT>::ret_type>::value,
	    typename std::remove_reference<typename function_res_traits<FT>::ret_type>::type
	    >::type 
    {
		if(!check_if_over())
			::ff::rt::yield_and_ret_until([this](){return check_if_over();});
        return deduct_t::ret_func_and(std::forward<FT>(f), m_1, m_2);
    }
    auto get() -> typename deduct_t::wrapper_type
    {
        return deduct_t::wrap_ret_values(m_1, m_2);
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
    T1_t  m_1;
    T2_t  m_2;
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
		, m_iES(exe_state::exe_unknown){}
    wait_or(const wait_or<T1, T2>& w)
        : m_1(w.m_1)
        , m_2(w.m_2)
		, m_iES(w.m_iES){
    }

    template<class FT>
    auto  then(FT && f)
    -> typename std::enable_if<std::is_void<typename function_res_traits<FT>::ret_type>::value, void>::type
    {
		if(!check_if_over())
			::ff::rt::yield_and_ret_until([this](){return check_if_over();});
        deduct_t::void_func_or(std::forward<FT>(f), m_1, m_2);
    }

    template<class FT>
    auto  then(FT && f ) ->
    typename std::enable_if<
	      !std::is_void<typename function_res_traits<FT>::ret_type>::value,
	    typename std::remove_reference<typename function_res_traits<FT>::ret_type>::type
	    >::type 
    {
		if(!check_if_over())
			::ff::rt::yield_and_ret_until([this](){return check_if_over();});
        return deduct_t::ret_func_or(std::forward<FT>(f), m_1, m_2);
    }
    auto get() -> typename deduct_t::wrapper_type
    {
        return deduct_t::wrap_ret_values(m_1, m_2);
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
    T1  m_1;
    T2  m_2;
    exe_state	m_iES;
};//end class wait_or


class wait_all
{
public:
	typedef void ret_type;
	wait_all(std::shared_ptr<std::vector<para<void> > >  ps)
	: all_ps(ps)
	, m_iES(exe_state::exe_unknown){};
	
	
	template<class FT>
    auto  then(FT && f ) -> void
    {
		if(!check_if_over())
			::ff::rt::yield_and_ret_until([this](){return check_if_over();});
		f();
    }


    exe_state	get_state()
	{
		if(m_iES != exe_state::exe_over)
		{
			m_iES = exe_state::exe_over;
			for(auto p= all_ps->begin(); p != all_ps->end(); ++p)
			{
				m_iES = m_iES && p->get_state();
			}
		}
		return m_iES;
	}
	bool	check_if_over()
	{
		if(m_iES == exe_state::exe_over)
			return true;
		get_state();
		if(m_iES == exe_state::exe_over)
			return true;
		return false;
	}
	
protected:
	std::shared_ptr<std::vector<para<void> > >  all_ps;
	exe_state	m_iES;
};//end class wait_all

class wait_any
{
public:
	typedef void ret_type;
	wait_any(std::shared_ptr<std::vector<para<void> > >  ps)
	: all_ps(ps)
	, m_iES(exe_state::exe_unknown){};
	
	
	template<class FT>
    auto  then(FT && f ) -> void
    {
		if(!check_if_over())
			::ff::rt::yield_and_ret_until([this](){return check_if_over();});
		f();
    }


    exe_state	get_state()
	{
		if(m_iES != exe_state::exe_over)
		{
			m_iES = exe_state::exe_unknown;
			for(auto p = all_ps->begin(); p != all_ps->end(); ++p)
				m_iES = m_iES || p->get_state();
		}
		return m_iES;
	}
	bool	check_if_over()
	{
		if(m_iES == exe_state::exe_over)
			return true;
		get_state();
		if(m_iES == exe_state::exe_over)
			return true;
		return false;
	}
	
protected:
	std::shared_ptr<std::vector<para<void> > >  all_ps;
	exe_state	m_iES;
};//end class wait_any


}//end namespace internal

template<class T>
struct is_para_or_wait: public std::false_type{};
template<class T>
struct is_para_or_wait<para <T> > : public std::true_type{};
template<class T1, class T2>
struct is_para_or_wait<internal::wait_and<T1, T2> > : public std::true_type{};
template<class T1, class T2>
struct is_para_or_wait<internal::wait_or<T1, T2> > : public std::true_type{};
template<>
struct is_para_or_wait<internal::wait_all> : public std::true_type{};
template<>
struct is_para_or_wait<internal::wait_any> : public std::true_type{};

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

auto all(paragroup & pg) -> internal::wait_all
{
	return internal::wait_all(pg.all_entities());
}

auto any(paragroup & pg) -> internal::wait_any
{
	return internal::wait_any(pg.all_entities());
}

}//end namespace ff

#endif
