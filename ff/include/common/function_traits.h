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