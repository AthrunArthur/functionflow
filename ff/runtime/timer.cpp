#include "utilities/timer.h"
#include "runtime/env.h"
#include "runtime/runtime.h"

namespace ff
{
  timer::timer()
    : m_timers()
  {
    auto thrd_num = ff::rt::hardware_concurrency();
    for(int i = 0; i<timer_index::timer_count; ++i)
    {
      m_timers.push_back(new single_timer(thrd_num));
    }
  }
  timer::~timer()
  {
    for(size_t i = 0; i < m_timers.size(); ++i)
    {
      delete m_timers[i];
    }
  }

  std::map<std::string, single_timer::duration_t> timer::get_durations()
  {
    std::map<std::string, single_timer::duration_t> res;
    std::map<timer_index, std::string> idxstr;
    idxstr[timer_index::para_timer] = "para-time";
    idxstr[timer_index::dep_timer] = "dep-time";
    idxstr[timer_index::schedule_timer] = "schedule-time";
    idxstr[timer_index::user_timer] = "user-time";
    idxstr[timer_index::total_timer] = "total-time";
    idxstr[timer_index::runner_timer] = "runner-time";
    for(std::map<timer_index, std::string>::iterator it = idxstr.begin(); it != idxstr.end(); ++it)
    {
      auto idx = it->first;
      auto name = it->second;
      res[name] = m_timers[idx]->get_duration();
    }
    return res;
  }

timer* g_timer = nullptr;
void init_timer()
{
  g_timer = new timer();
}

timer & timer_instance()
{
  return *g_timer;
}

}//end namespace ff
