#ifndef FF_UTILITIES_TIMER_H_
#define FF_UTILITIES_TIMER_H_
#include "common/common.h"
#include "utilities/single_timer.h"
#include <map>

namespace ff
{
  class timer
  {
    public:
      enum timer_index{
        para_timer = 1,
        dep_timer,
        schedule_timer,
        user_timer,
        total_timer,
        runner_timer,
        timer_count,
      };

      timer();
      ~timer();
      template<timer_index N>
      void start()
      {
        m_timers[N]->start();
      }

      template<timer_index N>
      void pause()
      {
        m_timers[N]->pause();
      }
      template<timer_index N>
      void all_reset()
      {
        m_timers[N]->all_reset();
      }
      template<timer_index N>
      void all_pause()
      {
        m_timers[N]->all_pause();
      }
      template<timer_index N>
      void append(const single_timer::duration_t & d)
      {
        m_timers[N]->append_duration(d);
      }

      std::map<std::string, single_timer::duration_t> get_durations();

    protected:
      typedef single_timer * single_timer_t;
      std::vector<single_timer_t> m_timers;
  };//end class timer

void  init_timer();
timer & timer_instance();

}//end namespace ff


#endif
