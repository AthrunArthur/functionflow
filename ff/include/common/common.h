#ifndef FF_COMMON_COMMON_H_
#define FF_COMMON_COMMON_H_

#include <cstdint>
#include <functional>
#include <atomic>
#include <thread>
#include <type_traits>
#include <cassert>
#ifdef FUNCTION_FLOW_DEBUG
#include <iostream>
#endif

#define CACHE_LINE_SIZE 64

namespace ff {

enum exe_state {
    exe_wait,
    exe_over,
    exe_run,
    exe_unknown,
};
exe_state operator &&(exe_state e1, exe_state  e2);
exe_state operator ||(exe_state e1, exe_state e2);

}//end namespace ff


#endif
