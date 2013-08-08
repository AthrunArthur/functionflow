#include "runtime/env.h"
#include "runtime/rtcmn.h"
#include "runtime/runtime.h"
//#include "common/log.h"
#include <functional>
#include <thread>
#include <iostream>
namespace ff {
namespace rt {
static thread_local thrd_id_t id = 0;



thrd_id_t get_thrd_id()
{
	return id;
}

void set_local_thrd_id(thrd_id_t i)
{
  id = i;
}
}//end namespace rt
}//end namespace ff
