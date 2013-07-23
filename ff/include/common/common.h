#ifndef FF_COMMON_COMMON_H_
#define FF_COMMON_COMMON_H_

#include <cstdint>
#include <functional>
#include <atomic>
#include <thread>
#include <type_traits>
#include <iostream>

namespace ff {

enum exe_state {
    exe_wait,
    exe_over,
    exe_run,
    exe_unknown,
};
exe_state operator &&(exe_state e1, exe_state  e2)
{
	if(e1 == e2)
		return e1;
	return exe_state::exe_unknown;
}
exe_state operator ||(exe_state e1, exe_state e2)
{
	if(e1 == exe_state::exe_over ||
		e2 == exe_state::exe_over
	)
		return exe_state::exe_over;
		return exe_state::exe_unknown;
}

}//end namespace ff


#endif