#include "ff.h"
#include <cmath>
#include <boost/iterator/iterator_concepts.hpp>
#include <chrono>
#include <iostream>
#include <mutex>
#include <vector>

using namespace std;

using namespace ff;

typedef std::mutex TMutex;
typedef std::shared_ptr<TMutex> TMutex_ptr;
typedef std::shared_ptr<int64_t> Res_ptr;
std::vector<TMutex_ptr> ms;
std::vector<Res_ptr> rs;

const int LOOP_TIMES = 1000;

int fib(int n)
{
  if(n <=2)
    return 1;
  return fib(n-1) + fib(n-2);
}

int random_fib()
{
  int i = rand()%25 + 10;
  return fib(i);
}

void task_fun(int j){
  for(int i = 0; i < LOOP_TIMES; ++i)
  {
    random_fib();
    ms[j]->lock();
    *(rs[j]) += random_fib();
    ms[j]->unlock();
  }
}

int main(int argc, char *argv[])
{
  para<> a;
  a([](){std::cout<<"this is for initialization"<<std::endl;});
  ff_wait(a);
  
  for(int i = 0; i< ff::rt::rt_concurrency(); i++)
  {
    ms.push_back(std::make_shared<TMutex>());
    rs.push_back(std::make_shared<int64_t>(0));
  }
  std::chrono::time_point<chrono::system_clock> start, end;
  
  start = std::chrono::system_clock::now();
  paragroup p;
  for(int i=0; i < ff::rt::rt_concurrency(); i++)
  {
    for(int j = 0; j < ff::rt::rt_concurrency(); j++)
    {
      para<> ptf;
      
      ptf([j](){task_fun(j);}/*, ms[j]->id()*/);
      //ptf.get_internal_impl()->hold_mutex = ms[j]->id();
      p.add(ptf);
    }
  }
  
  ff_wait(all(p));
  end = std::chrono::system_clock::now();
  int elapsed_seconds = std::chrono::duration_cast<chrono::microseconds>
                          (end-start).count();
  
  std::cout<<"elapsed time: "<<elapsed_seconds<<std::endl;
  
  return 0;
}