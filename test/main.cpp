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
#include <iostream>
#include <memory>
#include <type_traits>

int g_iCounter = 0;
class A{
public:
	A(): i(g_iCounter){g_iCounter ++ ; std::cout<<i<<std::endl;}
	~A(){std::cout<<"dtor "<<i<<std::endl;}
	
protected:
	int i;
};

typedef A B[1];

template<class T>
struct array_len {};

template<class T, size_t N>
struct array_len<T[N]> {
	static const size_t len = N;
};

typedef std::shared_ptr<typename std::remove_extent<B>::type> B_ptr;

void f(B b)
{
	std::cout<<"f(B b)"<<std::endl;
}
int main()
{
	B_ptr a(new A[array_len<B>::len], [](A * p){delete[] p;});
	f(a.get());
	return 0;
}