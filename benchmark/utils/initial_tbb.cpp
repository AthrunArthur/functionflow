#include "initial_para_env.h"
#include <tbb/parallel_for_each.h>
#include <tbb/task_scheduler_init.h>
// #include <tbb/concurrent_vector.h>//concurrent_vector
#include "tbb/task_group.h"//task_group

using namespace tbb;

void initial_para_env(int thrd_num)
{
  task_scheduler_init init(8);//Construct task scheduler with p threads
  tbb::task_group tg;
  for(int i = 0; i < 5; ++i)
    tg.run([&i]() {});
  tg.wait();
}

