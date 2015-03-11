#include "ff.h"
#include "common/log.h"

void initial_para_env(int thrd_num)
{
  ff::rt::set_hardware_concurrency(8);//Set concurrency
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
