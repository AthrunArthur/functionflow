#ifndef FF_UTILITIES_SINGLE_TIMER_H_
#define FF_UTILITIES_SINGLE_TIMER_H_

#include "common/common.h"
#include <chrono>
#include <vector>
#include <string>

namespace ff
{
class single_timer
{
  public:
    typedef std::chrono::duration<double> duration_t;
    typedef std::chrono::high_resolution_clock st_clock_t;
    typedef std::chrono::time_point<st_clock_t> time_point_t;

    single_timer(int thrd_num);
    void start();
    void pause();
    void all_pause();
    void all_reset();
    void append_duration(const duration_t  & d);

    duration_t get_duration();

  protected:
    std::vector<time_point_t> m_starts;
    std::vector<duration_t> m_durations;

    static time_point_t s_zero_value;
};//end class single_timer
}//end namespace ff
#endif
