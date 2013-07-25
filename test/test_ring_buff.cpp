#include "runtime/ring_buff.h"

#include <iostream>
using namespace ff::internal;
int main()
{
	seq_num<256> t;
	t.increment();
	std::cout<<t.num().load()<<std::endl;
	t.decrement();
	t.sub(5);
	std::cout<<t.num().load()<<std::endl;

	ring_buff<int, 8> rb;
	rb.push_front(1);
	rb.push_front(2);
	rb.push_front(3);
	rb.push_front(4);
	int tv;
	std::cout<<rb.size()<<std::endl;
	bool b = rb.pop_front(tv);
	std::cout<<"shold be true: 4-- "<<b<<" "<<tv<<std::endl;
	std::cout<<"size (3) -->"<<rb.size()<<std::endl;
	b = rb.pop_back(tv);
	std::cout<<"should be (true:1) -->"<<b<<" "<<tv<<std::endl;
	std::cout<<"size (2) -->"<<rb.size()<<std::endl;
	return 0;
}
