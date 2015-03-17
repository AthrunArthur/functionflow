#include "utilities/single_timer.h"
#include "runtime/runtime.h"
#include "runtime/env.h"

namespace ff
{
  single_timer::time_point_t single_timer::s_zero_value = single_timer::st_clock_t::now();

  single_timer::single_timer(int thrd_num)
    : m_starts()
    , m_durations(){
      for(int i = 0; i < thrd_num; ++i)
      {
        m_starts.push_back(s_zero_value);
        m_durations.push_back(duration_t(0));
      }
    }
  void single_timer::start()
  {
    thread_local static thrd_id_t thrd_id = ff::rt::get_thrd_id();
    auto n = st_clock_t::now();/*
    if(m_starts[thrd_id] != s_zero_value)
    {
      m_durations[thrd_id] += (n-m_starts[thrd_id]);

      m_starts[thrd_id] = n;
    }else*/
      m_starts[thrd_id] = n;
  }
  void single_timer::all_reset()
  {
    auto n = st_clock_t::now();
    for(size_t i = 0; i < m_starts.size(); ++i)
      m_starts[i] = n;
  }
  void single_timer::pause()
  {
    thread_local static thrd_id_t thrd_id = ff::rt::get_thrd_id();
    auto n = st_clock_t::now();
    m_durations[thrd_id] += (n-m_starts[thrd_id]);
    m_starts[thrd_id] = s_zero_value;
  }
  void single_timer::append_duration(const duration_t & d)
  {
    thread_local static thrd_id_t thrd_id = ff::rt::get_thrd_id();
    m_durations[thrd_id] += d;
  }
  void single_timer::all_pause()
  {
    auto n = st_clock_t::now();
    for(size_t i = 0; i < m_starts.size(); ++i)
    {
      m_durations[i] += (n-m_starts[i]);
      m_starts[i] = s_zero_value;
    }
  }

  single_timer::duration_t single_timer::get_duration()
  {
    duration_t r(0);
    for(size_t i = 0; i < m_durations.size(); ++i)
    {
      r += m_durations[i];
    }
    return r;
  }
  
}//end namespace ff
