#ifndef BENCHMARK_RRECORD_H_
#define BENCHMARK_RRECORD_H_

#include <chrono>
#include <iostream>
#include <sstream>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <string>
#ifdef COUNT_TIME
#include "utilities/timer.h"
#endif

class RRecord{
  public:
    RRecord(const std::string & fp, const std::string & tag);
    ~RRecord();

    template <class Func_t, typename... Args_t>
      void run(const std::string & prefix, Func_t && f, Args_t&...   args)
      {
        std::chrono::time_point<std::chrono::system_clock> start, end;
        start = std::chrono::system_clock::now();
#ifdef COUNT_TIME
        ff::timer_instance().all_reset<ff::timer::total_timer>();
#endif
        f(args...);
#ifdef COUNT_TIME
  ff::timer_instance().all_pause<ff::timer::total_timer>();
#endif
        end = std::chrono::system_clock::now();
        auto elapsed_seconds = std::chrono::duration_cast<std::chrono::microseconds>(end-start).count();
        m_PT.put(prefix, elapsed_seconds);
      }

    template <class T>
    void put(const std::string & prefix, const T & v)
    {
      m_PT.put(prefix, v);
    }


  protected:
    boost::property_tree::ptree  m_PT;
    std::string m_strFP;

};//end class RRecord

#endif
