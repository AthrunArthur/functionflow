#include <iostream>

class A{};

A&& func(A &t)
{
	std::cout<<5<<std::endl;
	return A();
}

A&& func(A &&t)
{
	std::cout<<5<<std::endl;
	return A();
}


int main()
{
	A a;
	A &&b = func(func(a));
	A && c = func(a);
	return 0;
}