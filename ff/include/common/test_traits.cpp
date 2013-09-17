#include "function_traits.h"
#include <iostream>

template<class F>
void print_property(F && f):
void f(int a){}

int main()
{
	std::cout<<ff::utils::is_no_args_function<decltype(main)>::value<<std::endl;
	std::cout<<ff::utils::is_no_args_function<decltype(f)>::value<<std::endl;
return 0;
}
