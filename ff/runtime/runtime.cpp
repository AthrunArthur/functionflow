#include "runtime/rtcmn.h"

namespace ff{
namespace rt{

void schedule(task_base_ptr p)
{	runtime_ptr r = runtime::instance();
	r->schedule(p);
}

void yield()
{
	std::this_thread::yield();
}

}//end namespace rt
}//end namespace ff
