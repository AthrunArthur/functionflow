#ifndef FF_COMMON_FUNCTION_TRAITS_H_
#define FF_COMMON_FUNCTION_TRAITS_H_
#include <functional>
#include <type_traits>

namespace ff {
namespace utils
{
template<class F>
struct deduce_function{};

template<class Ret, class C, class... Args>
struct deduce_function<Ret (C::*)(Args...) const>
{
    typedef std::function<Ret (Args...)> type;
    typedef Ret ret_type;
};

template<class Ret, class C, class... Args> 
struct deduce_function<Ret (C::*)(Args...)> 
{ 
   typedef std::function<Ret(Args...)> type; 
   typedef Ret		ret_type;
};

template<class F>
struct function_res_traits
{
	typedef typename deduce_function<decltype(&std::remove_reference<F>::type::operator())>::ret_type ret_type;
};

}//end namespace utils
};//end namespace ff

#endif