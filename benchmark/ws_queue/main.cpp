#include <thread>
#include <cstdlib>
#include <iostream>
#include <mutex>

#ifdef USE_CPP11_ATOMIC
#include "runtime/work_stealing_queue.h"
typedef ff::rt::work_stealing_queue<int64_t, 8> WSQ_t;
#endif

#ifdef USE_GCC_ATOMIC
#include "runtime/gtwsq_fixed.h"
typedef ff::rt::gcc_work_stealing_queue<int64_t, 8> WSQ_t;
#endif

#ifdef USE_SPIN
#include "runtime/spin_steal_queue.h"
typedef ff::rt::spin_stealing_queue<int64_t, 8> WSQ_t;
#endif

  template <class Func_t, typename... Args_t>
void run(const std::string & prefix, Func_t && f, Args_t&&...   args)
{
  std::chrono::time_point<std::chrono::system_clock> start, end;
  start = std::chrono::system_clock::now();
  f(args...);
  end = std::chrono::system_clock::now();
  auto elapsed_seconds = std::chrono::duration_cast<std::chrono::microseconds>(end-start).count();
  std::cout<<prefix<<" : "<<elapsed_seconds<<std::endl;
}

int interval_push_pop(int64_t counts)
{
  WSQ_t q;
  int64_t t;
  for(int64_t i = 0; i < counts; ++i)
  {
    q.push_back(i);
    q.pop(t);
  }
  return 0;
}

int random_push_pop(int64_t counts)
{
  WSQ_t q;
  int64_t t;
  int64_t i = 0;
  while(i < counts )
  {
    int64_t k = std::rand()%(1<<8);
    int64_t ki = 0;
    while(ki <k && q.push_back(ki)) ki ++;

    k = std::rand()%(1<<8);
    ki = 0;
    while(ki < k && q.pop(t)) ki ++;
    i ++;
  }
  return 0;
}

std::mutex g_print_mutex;

void multithread_push_pop(int64_t counts)
{
  std::atomic_bool start(false);
  std::atomic_bool quit(false);
  int64_t push_sum = 0;
  int64_t pop_sum1 = 0, pop_sum2 = 0;
  WSQ_t q;
  std:: thread thrd1([&start, counts, &push_sum, &pop_sum1, &q, &quit](){
      while(!start) std::this_thread::yield();
      std::chrono::time_point<std::chrono::system_clock> start, end;
      start = std::chrono::system_clock::now();
      for(int64_t i =0; i < counts; ++i){
        if(q.push_back(i))
          push_sum += i;
        if(i % 3 == 0)
        {
          int64_t t;
          if(q.pop(t)) pop_sum1 += t;
        }
      }
      while(q.size() != 0){
      int64_t t;
      if(q.pop(t)) pop_sum1 +=t;
      }
      quit = true;
      end = std::chrono::system_clock::now();
      auto elapsed_seconds = std::chrono::duration_cast<std::chrono::microseconds>(end-start).count();
      g_print_mutex.lock();
      std::cout<<"push/pop thread : "<<elapsed_seconds<<std::endl;
      g_print_mutex.unlock();
      });

  std::thread thrd2([&start, &pop_sum2, &q, & quit](){
      while(!start) std::this_thread::yield();
      std::chrono::time_point<std::chrono::system_clock> start, end;
      start = std::chrono::system_clock::now();
      while(!quit)
      {
        int64_t t;
        if(q.steal(t)){
          pop_sum2 += t;
        }
      }
      end = std::chrono::system_clock::now();
      auto elapsed_seconds = std::chrono::duration_cast<std::chrono::microseconds>(end-start).count();
      g_print_mutex.lock();
      std::cout<<"steal thread : "<<elapsed_seconds<<std::endl;
      g_print_mutex.unlock();
      });
  start = true;

  thrd1.join();
  thrd2.join();

  if(push_sum != pop_sum1 + pop_sum2)
  {
    std::cout<<"bugs here! push sum is: "<<push_sum<<", pop_sum is "<<pop_sum1 + pop_sum2<<std::endl;
  }

}

int main(int argc, char *argv[])
{
  run("interval_push_pop", interval_push_pop, 1<<20);
  run("random_push_pop", random_push_pop, 1<<15);
  multithread_push_pop(1<<20);
  return 0;
}
