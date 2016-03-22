#include "ff.h"
#ifdef COUNT_TIME
#include "utilities/timer.h"
#endif

void initial_para_env(int thrd_num)
{
  ff::initialize(thrd_num);//Set concurrency
#ifdef COUNT_TIME
  ff::init_timer();
#endif
  ff::para<int> a;
  int num = 10;
  a([&num]() {
      return num;
      }).then([](int x) {});
  ff::para<> b;
  b[a]([&num](int ares) {
      num + ares;
      }).then([]() {});
}
