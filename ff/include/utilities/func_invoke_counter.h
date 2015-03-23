#ifndef FF_UTILITIES_FUNC_INVOKE_COUNTER_H_
#define FF_UTILITIES_FUNC_INVOKE_COUNTER_H_
#include "common/common.h"
#include <vector>
#include <map>
#include "utilities/thread_local_var.h"

namespace ff
{
  class func_invoke_counter
  {
    public:
      enum{
#define FUNC_TAG(a) a,
#include "utilities/func_tags.h"
#undef FUNC_TAG
      };

    static void call(int tag);
    static bool is_normal();
    static void init(int thrd_num);
    static std::string status();
    static std::map<int, std::string> tags();
    protected:
      typedef std::vector<uint64_t> counters_t;
      static thread_local_var<counters_t> m_oCounters;
  };//end class func_invoke_counter
}//end namespace ff
#endif
