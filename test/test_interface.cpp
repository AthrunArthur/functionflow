#include <iostream>
#include <vector>
#include "ff.h"

using namespace ff;

int foo(int t)
{
	std::cout<<"foo:"<<t<<std::endl;
	return t + 1;
}
double bar(double t)
{
	std::cout<<"bar:"<<t<<std::endl;
	return t + 1;
}


int fib(int n)
{
	if(n <=2)
		return 1;
	ff::para<int> a, b;
	a([&n]()->int{return fib(n - 1);});
	b([&n]()->int{return fib(n - 2);});
	return (a && b).then([](int x, int y){return x + y;});
}

int main(int argc, char *argv[])
{	
	
	ff::para<int> a;
	int num = 10;
	a([&num](){return foo(num);}).then([](int x){std::cout <<"got x:"<<x<<std::endl;});
	ff::para<> b;
	b[a]([&num, &a](){foo(num + a.get());}).then([](){std::cout<<"do anything here!"<<std::endl;});
	
	
	int fib_res = fib(num);
	std::cout<<"fib( "<<num<<" )="<<fib_res<<std::endl;
	
	ff::para<int> f1;
	ff::para<double> f2;
	f1([](){return foo(1);});
	f2([](){return bar(2.2);});
	(f1 || f2).then([](int index, std::tuple<int, double> res){
		std::cout<<"(f1||f2).then "<<std::get<1>(res)<<std::endl;}
	);
	
	ff::para<> f3;
	f3([](){return foo(3);});
	ff::para<double> f4;
	f4[f3]([](){return bar(4.5);}).then([](double x){std::cout<<"f4.then "<<x<<std::endl;});
	(f3 && f4).then([](double x){std::cout<<"f3&&f4.then "<<x<<std::endl;});
	
	
	std::vector<int> s;
	s.push_back(10);
	s.push_back(11);
	s.push_back(15);
	s.push_back(9);
	ff::paragroup<void> pg;
	pg.for_each(s.begin(), s.end(), [](int x){std::cout<<"para group: "<<x<<std::endl;});
	
	ff::accumulator<int> sum(0, [](const int & x, const int& y){return x + y;});
	ff::paragroup<void> pg1;
	pg1.for_each(s.begin(), s.end(), [&sum](int x){
	  sum.increase(x);
	});
	std::cout<<"pg1 sum: "<<sum.get()<<std::endl;
	
	ff::paragroup<void> pg2;
	ff::single_assign<int> first;
	pg2.for_each(s.begin(), s.end(), [&first](int x){first = x;});
	std::cout<<"pg2 first: "<<first.get()<<std::endl;
	return 0;
}
