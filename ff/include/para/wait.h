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
	typedef typename bin_wait_func_deducer<T1, T2>::and_type ret_type;
public:
    wait_and(T1 && t1, T2 && t2)
	: m_1(t1)
	, m_2(t2){}

    template<class FT>
    auto  then(const FT & f)
	-> typename std::enable_if<std::is_void<typename function_res_traits<FT>::ret_type>::value, void>::type
    {
		bin_wait_func_deducer<T1, T2>::void_func_and(f, m_1, m_2);
    }

    template<class FT>
    auto  then(const FT & f ) ->
    typename std::remove_reference<typename function_res_traits<FT>::ret_type>::type &&
    {
		return bin_wait_func_deducer<T1, T2>::ret_func_and(f, m_1, m_2);
    }
    auto get() -> typename bin_wait_func_deducer<T1, T2>::wrapper_type
	{
		return bin_wait_func_deducer<T1, T2>::wrap_ret_values(m_1, m_2);
	}
protected:
	T1 & m_1;
	T2 & m_2;
};//end class wait_and


template <class T1, class T2>
class wait_or
{
public:
	typedef typename bin_wait_func_deducer<T1, T2>::or_type ret_type;
public:
    wait_or(T1 & t1, T2 & t2)
	: m_1(t1)
	, m_2(t2){}
	wait_or(const wait_or<T1, T2>& w)
	: m_1(w.m_1)
	, m_2(w.m_2){
	}

    template<class FT>
    auto  then(const FT & f)
	-> typename std::enable_if<std::is_void<typename function_res_traits<FT>::ret_type>::value, void>::type
    {
		bin_wait_func_deducer<T1, T2>::void_func_or(f, m_1, m_2);
    }

    template<class FT>
    auto  then(const FT & f ) ->
    typename std::remove_reference<typename function_res_traits<FT>::ret_type>::type &&
    {
		return bin_wait_func_deducer<T1, T2>::ret_func_or(f, m_1, m_2);
    }
    auto get() -> typename bin_wait_func_deducer<T1, T2>::wrapper_type
	{
		return bin_wait_func_deducer<T1, T2>::wrap_ret_values(m_1, m_2);
	}
	
protected:
	T1 & m_1;
	T2 & m_2;
};//end class wait_or

}//end namespace internal
template<class T1, class T2>
internal::wait_and<T1, T2> operator &&(T1 & t1, T2 & t2)
{
    return internal::wait_and<T1, T2>(std::forward<T1>(t1), std::forward<T2>(t2));
}

template<class T1, class T2>
internal::wait_or<T1, T2> operator ||(T1 & t1, T2 & t2)
{
    return internal::wait_or<T1,T2>(t1, t2);
}


#if 0
namespace internal
{
template <class T1, class T2>
class wait_and
{
public:
    wait_and(T1 & t1, T2 & t2) {}
    wait_and(T1 && t1, T2 && t2) {}

protected:

};//end class wait_and
template <class T1, class T2>
class wait_or
{
public:
    wait_or(T1 & t1, T2 & t2) {}
    wait_or(T1 && t1, T2 && t2) {}
};//end class wait_or

class wait_all
{
public:
    wait_all(paragroup & pg) {}
    wait_all(paragroup && pg) {}
};//end class wait_all

class wait_any
{
public:
    wait_any(paragroup & pg) {}
    wait_any(paragroup && pg) {}
};//end class wait_any


}//end namespace internal

template<class T1, class T2>
internal::wait_and<T1, T2>&& operator &&(T1 & t1, T2 & t2)
{
    return internal::wait_and<T1, T2>(t1, t2);
}
template<class T1, class T2>
internal::wait_and<T1, T2>&& operator &&(T1 && t1, T2 && t2)
{
    return internal::wait_and<T1, T2>(t1, t2);
}
template<class T1, class T2>
internal::wait_or<T1, T2>&& operator ||(T1 & t1, T2 & t2)
{
    return internal::wait_or<T1, T2>(t1, t2);
}
template<class T1, class T2>
internal::wait_or<T1, T2>&& operator ||(T1 && t1, T2 && t2)
{
    return internal::wait_or<T1, T2>(t1, t2);
}

internal::wait_all all(paragroup & pg)
{
    return internal::wait_all(pg);
}
internal::wait_all all(paragroup &&pg)
{
    return internal::wait_all(pg);
}
internal::wait_any any(paragroup & pg)
{
    return internal::wait_any(pg);
}
internal::wait_any any(paragroup && pg)
{
    return internal::wait_any(pg);
}
#endif
}//end namespace ff

#endif