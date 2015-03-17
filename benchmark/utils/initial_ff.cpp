#include "ff.h"
#include "common/log.h"
#ifdef COUNT_TIME
#include "utilities/timer.h"
#endif

void initial_para_env(int thrd_num)
{
  ff::rt::set_hardware_concurrency(thrd_num);//Set concurrency
#ifdef COUNT_TIME
  ff::init_timer();
#endif
  _DEBUG(ff::fflog<>::init(ff::INFO, "log.txt"))
    _DEBUG(LOG_INFO(main)<<"main start, id:"<<ff::rt::get_thrd_id());
  ff::para<int> a;
  int num = 10;
  a([&num]() {
      return num;
      }).then([](int x) {});
  ff::para<> b;
  b[a]([&num, &a]() {
      num + a.get();
      }).then([]() {});
}
