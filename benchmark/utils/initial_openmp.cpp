#include "initial_para_env.h"

#include <omp.h>

void initial_para_env(int thrd_num)
{
  omp_set_num_threads(8);
}
