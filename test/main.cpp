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