#ifndef FF_PARA_BIN_WAIT_FUNC_DEDUCER_H_
#define FF_PARA_BIN_WAIT_FUNC_DEDUCER_H_
#include "common/common.h"
#include <tuple>


namespace ff {
	template<class RT>
	class para;
namespace internal
{
template <class T1, class T2>
struct bin_wait_func_deducer{
	typedef std::tuple<typename T1::ret_type, typename T2::ret_type> pair;
	typedef pair and_type;
	typedef std::tuple<int, pair> or_type;
	typedef pair && wrapper_type;
	
	template<class FT>
	static void 	void_func_and( const FT & f, T1 & t1, T2 & t2){f(t1.get(), t2.get());}
	template<class FT>
	static auto	ret_func_and(const FT & f, T1 & t1, T2 & t2)
	-> typename std::remove_reference<typename function_res_traits<FT>::ret_type>::type 
	{return f(t1.get(), t2.get());}
	
	template<class FT>
	static void 	void_func_or( const FT & f, T1 & t1, T2 & t2){
		std::cout<<t1.get()<<" "<<t2.get()<<std::endl;
		f(0, std::make_tuple(t1.get(), t2.get()));}
	template<class FT>
	static auto	ret_func_or(const FT & f, T1 & t1, T2 & t2)
	-> typename std::remove_reference<typename function_res_traits<FT>::ret_type>::type &&
	{return f(0, std::make_tuple(t1.get(), t2.get()));}
	
	pair &&  wrap_ret_values(T1 & t1, T2 & t2)
	{
		return pair(t1.get(), t2.get());
	}
};//end struct bin_wait_func_deducer;
template < class T2>
struct bin_wait_func_deducer<para<void>, T2>{
	typedef typename T2::ret_type pair;
	typedef pair and_type;
	typedef pair or_type;
	typedef typename std::remove_reference<pair>::type && wrapper_type;
	template<class FT>
	static void 	void_func_and( const FT & f, para<void> & t1, T2 & t2){f(t2.get());}
	template<class FT>
	static auto	ret_func_and(const FT & f, para<void> & t1, T2 & t2)
	-> typename std::remove_reference<typename function_res_traits<FT>::ret_type>::type &&
	{return f(t2.get());}
	
	template<class FT>
	static void 	void_func_or( const FT & f, para<void> & t1, T2 & t2){f(t2.get());}
	template<class FT>
	static auto	ret_func_or(const FT & f, para<void> & t1, T2 & t2)
	-> typename std::remove_reference<typename function_res_traits<FT>::ret_type>::type &&
	{return f(t2.get());}
	
	wrapper_type  wrap_ret_values(para<void> & t1, T2 & t2)
	{
		return pair(t2.get());
	}
};//end struct bin_wait_func_deducer;
template <class T1>
struct bin_wait_func_deducer<T1, para<void>>{
	typedef typename T1::ret_type pair;
	typedef pair and_type;
	typedef pair or_type;
	typedef typename std::remove_reference<pair>::type && wrapper_type;
	template<class FT>
	static void 	void_func_and( const FT & f, T1 & t1, para<void> & t2){f(t1.get());}
	template<class FT>
	static auto	ret_func_and(const FT & f, T1 & t1, para<void> & t2)
	-> typename std::remove_reference<typename function_res_traits<FT>::ret_type>::type &&
	{return f(t1.get());}
	
	template<class FT>
	static void 	void_func_or( const FT & f, T1 & t1, para<void> & t2){f(t1.get());}
	template<class FT>
	static auto	ret_func_or(const FT & f, T1 & t1, para<void> & t2)
	-> typename std::remove_reference<typename function_res_traits<FT>::ret_type>::type &&
	{return f(t1.get());}
	
	wrapper_type  wrap_ret_values(T1 & t1, para<void> & t2)
	{
		return pair(t1.get());
	}
};//end struct bin_wait_func_deducer;

template <>
struct bin_wait_func_deducer<para<void>, para<void>>{
	typedef void pair;
	typedef pair and_type;
	typedef pair or_type;
	typedef pair wrapper_type;
	
	template<class FT>
	static void 	void_func_and( const FT & f, para<void> & t1, para<void> & t2){f();}
	template<class FT>
	static auto	ret_func_and(const FT & f, para<void> & t1, para<void> & t2)
	-> typename std::remove_reference<typename function_res_traits<FT>::ret_type>::type &&
	{return f();}
	
	template<class FT>
	static void 	void_func_or( const FT & f, para<void> & t1, para<void> & t2){f();}
	template<class FT>
	static auto	ret_func_or(const FT & f, para<void> & t1, para<void> & t2)
	-> typename std::remove_reference<typename function_res_traits<FT>::ret_type>::type &&
	{return f();}
	
	void  wrap_ret_values(para<void> & t1, para<void> & t2){}
};//end struct bin_wait_func_deducer;

}//end namespace internal
}//end namespace ff
#endif
