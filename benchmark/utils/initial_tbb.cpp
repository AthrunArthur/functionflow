#include "initial_para_env.h"
#include <tbb/parallel_for_each.h>
#include <tbb/task_scheduler_init.h>
// #include <tbb/concurrent_vector.h>//concurrent_vector
#include "tbb/task_group.h"//task_group

using namespace tbb;
task_scheduler_init * pInit;
void initial_para_env(int thrd_num)
{
  pInit = new task_scheduler_init(thrd_num);//Construct task scheduler with p threads
  tbb::task_group tg;
  for(int i = 0; i < 5; ++i)
    tg.run([&i]() {});
  tg.wait();
}

