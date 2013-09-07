Function Flow使用说明
==========================
#1. Function Flow简介

Function Flow（FF）是一个全新设计的、统一的并行编程框架，主要用于简化以下编程问题：

1. 任务并行;
2. 数据并行;
3. 并行任务之间的依赖关系或等待关系;

FF的理念是尽可能静态的表达并行及依赖关系，这样可以使得

1. 及时发现更多的错误;
2. 使运行时系统可以更好的利用程序的特征，进行有针对性的优化

FF是免费且开源的，其实现仅依赖于完整的C++11实现，因此你需要一个支持C++11的编译器来编写FF程序，建议不低于G++4.8.1或Clang3.3,下面是一些FF使用到一些C++11特性，请确定你的编译器是否支持。

1. Lambda
2. thread, mutext，yield
3. 变长模板参数
4. 右值引用
5. atomic
6. functional, bind
7. tuple
8. type traits, 比如enable_if
9. 智能指针
10. auto（C++11）
11. for（C++11）
12. template的typedef
13. delete用于控制默认构造函数和赋值函数

#2. FF入门
本章介绍如何使用FF编写最简单的并行程序--Hello world。
##2.1 Hello world
首先，我们从最简单的Hello world开始，即如何使用并行的手段，输出hello world，并等待该并行程序的结束，代码如下

	#include "ff.h"
	#include <stdio.h>
	using namespace ff;

	int main(int argc, char *argv[])
	{
		para<> a;
		a([](){printf("hello world\n");});
		ff_wait(a);
		printf("goodbye world\n");

		return 0;
	}
使用FF时，必须包含必要的头文件"ff.h"，并引入必要的名字空间ff。 para<> a; 表示声明一个并行的任务a，此时，任务a没有绑定到任何函数调用，也不会产生任何开销。a实际上是一个函数对象(Functor)，在FF中，称之为para对象，其参数为另外一个可调用的对象，例如std::bind返回的函数对象，一个lambda表达式等。此处，并行的对象是一个lambda表达式，其中调用了printf("hello world\n")。ff_wait表示等待并行任务a的结束，此后，调用printf("goodbye world\n")。

注意，虽然我们在a中调用了一个lambda表达式，该表达式实际上并不在当前线程执行，而是在另外一个线程执行。
####关于ff::para
ff::para是FF用于表达并行的核心类型。在FF中，并行的基本单位是一个函数或Lambda表达式（这不同于OpenMP的代码段并行方式），其手段则是通过声明ff::para对象，并调用para对象上的方法。

ff::para的原型如下：

	template<class RT=void>
	class para;
其中RT表示需要被并行的函数的返回类型，例如：
	
	ff::para<int> pf;
	pf([]()->int{return 5;});
注意，即使被并行的函数的返回类型即使不为void，也可以在ff::para中使用void做为模板参数，反之则是非法的。详见其后对于then的解释。

如果ff::para中模板参数类型不为void，则可以使用get()方法得到最终的返回结果，例如

	ff::para<int> pf;
	pf([]()->int{return 5;});
	//...
	int res = pf.get();

##2.2 另一个Hello world
在前一个Hello world中，整个程序被分为两部分，首先通过并行输出"hello world\n"，在该并行任务结束后，输出"goodbye world\n"，即这两个任务之间存在显著的依赖关系。在FF中，通常使用then来表达这种“一个任务在另一个任务结束之后”的关系。下面是另一个版本的hello world。

	#include "ff.h"
	#include <stdio.h>
	using namespace ff;
	
	int main(int argc, char *argv[])
	{
		para<void> a;

		a([](){printf("hello world\n");}).then([](){
			printf("goodbye world\n");});
		return 0;
	}
a(f1).then(f2)表示，f1在另一个线程中执行，同时当前线程阻塞，直至a，即f1执行结束，则当前线程恢复，并执行f2。该版本的语义与上一个版本的hello world是一致的。
####关于then
then函数的参数同样是一个函数对象，需要注意的是该函数对象有一个参数，该参数的类型和ff::para的模板参数类型一致。例如
	
	ff::para<int> pf;
	pf([]()->int{return 5;}).then([](int res){   //res的值为5;
		printf("pf return %d", res);});
由于pf的声明中已经指定了模板参数的类型为int，则then中的函数对象的参数类型也必须为int，否则，会出现编译错误。此时then的语义为，当pf结束时，其结果被传递给then中的函数对象，并调用该函数对象，因此本例中输出5。

#3. FF中的依赖关系
本章通过Fibonacci介绍如何在FF中表达依赖关系及递归。
##3.1 Fibonacci
Fibonacci的计算通常使用迭代，而不是递归，本处仅将其递归实现作为一个简单的示例。代码如下，省略main函数及头文件
	
	int fib(int n)
	{
		if(n <=2)
			return 1;
		ff::para<int> a, b;
		a([&n]()->int{return fib(n - 1);});
		b([&n]()->int{return fib(n - 2);});
		return (a && b).then([](int x, int y){return x + y;});
	}
该例中，声明了a，b两个para对象，分别用来递归的计算n-1及n-2的Fib值，最后等待a,b的结束，并将其值求和做为返回值。其中

	(a && b).then([](int x, int y){return x + y;})
表示等待a及b的结束，在a，b结束后，调用then中的函数对象，该函数对象的参数分别为a,b的返回值。FF中既可以在并行函数上附加依赖关系作为其启动条件，也可以像本例中一样使用单独的表达式，还可以使用ff_wait。这些表达方式都依赖于依赖表达式。
##3.2 依赖表达式
简单来说，依赖表达式运行FF组合各种依赖关系。FF引入四种依赖关系表达式的算子，分别是&&, ||, any, all，其定义如下：

1. a && b，表示等待a以及b的结束;
2. a || b，表示等待a或b的结束;
3. any(s)，其中s表示paragroup，包含多个para对象，表示等待s中任意一个对象的结束;
4. all(s)，表示等待s中所有para对象的结束。

其中a，b表示依赖表达式或para对象，s表示paragroup（下文介绍）。在依赖表达式中可以使用（）改变优先级。
##3.3 在para对象中使用依赖表达式
在FF中，可以在使用para对象时指定该para对象开始的条件，即依赖表达式的结束，一个简单的例子如下
	
	para<void> a;
	a([](){printf("hello world\n");});
	para<void> b;
	b[a]([](){printf("goodbye world\n");});
	
类似与之前的hello world的例子，本例同样指定了两个任务的先后关系，不同的是，任务b是在另一个线程中执行，而不是当前线程。	 一个更复杂的例子如下，在下例中，我们等待任意任务的结束，并使用其返回值。

	para<int> a, b;
	a([]()->int{return 5;});
	b([]()->int{return 6;});
	para<int> c;
	c[a && b]([&a, &b](){printf("a+b=%d", a.get() + b.get());});
##3.4 使用单独的依赖表达式
在FF中，单独的依赖表达式可以配合then函数，在主线程中等待依赖表达式的结束，并执行then函数中的内容。需要注意的是，then函数中的函数对象的原型必须满足依赖表达式的限制。考虑形如

	(expr).then(f)
	
的依赖表达式使用，则expr与f的参数列表有如下关系，其中a，b分别为依赖表达式，其返回类型分别为TA，TB，记为

	a: ->TA; b: ->TB
	
则对于不同的expr与f有如下关系（*需要补充关于paragroup部分的函数原型问题*）

	expr:  a && b -----> f(TA, TB )
	expr:  a || b -----> f(int index, std::tuple<TA, TB>)
	
其中index为0则表示a结束，为1则表示b结束。特别的，若其中TA，或TB的类型为void，则f的类型自动退化，退化为非void类型参数。下面是一些表达式，及对应的f的原型。

例1：

	a: ->int; b: -> double;
	a && b ----> f(int, double)
	a || b ----> f(int index, std::tuple<int, double>)
	
例2：

	a: ->void; b: -> double;
	a && b ----> f(double)
	a || b ----> f(double)
	
例3：
 
	a: ->void; b: -> double; c: ->int; d: ->string;
	(a&&b) || (c&&d) -----> f(int index, std::tuple<double, std::tuple<int, string> >)
	(a&&b) && (c&&d) -----> f(double, int, string)
	(a||b) && (c||d) -----> f(double, std::tuple<int, std::tuple<int, string>>)
##3.5 使用ff_wait
ff_wait的参数为依赖表达式，用于在主线程中等待依赖表达式的结束。

#4. FF中的数据并行
#5. 并发数据结构
##5.1 ff::accumulator<T>
##5.2 ff::single_assign<T>
