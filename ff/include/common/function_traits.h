#ifndef FF_COMMON_FUNCTION_TRAITS_H_
#define FF_COMMON_FUNCTION_TRAITS_H_
#include <functional>
#include <type_traits>

class stat;
struct stat;
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


template<class Ret, class... Args> 
struct deduce_function<Ret (Args...)> 
{ 
   typedef std::function<Ret(Args...)> type; 
   typedef Ret		ret_type;
};


template<class F>
struct is_no_args_function {
  const static bool value = false;
};

template<class Ret, class C>
struct is_no_args_function<Ret (C:: *)(void) const>{
  const static bool value = true;
};

template<class Ret, class C>
struct is_no_args_function<Ret (C:: *)(void)> {
  const static bool value = true;
};

template<class Ret>
struct is_no_args_function<Ret (void)> {
  const static bool value = true;
};
template<class Ret>
struct is_no_args_function<Ret (*)(void)> {
  const static bool value = true;
};

template<class F>
struct function_res_traits
{
	typedef typename deduce_function<decltype(&std::remove_reference<F>::type::operator())>::ret_type ret_type;
};

template<class F>
struct function_args_traits
{
  const static bool is_no_args = is_no_args_function<decltype(&std::remove_reference<F>::type::operator())>::value;
};

}//end namespace utils
};//end namespace ff

#endif