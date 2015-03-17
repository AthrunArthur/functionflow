#include "rrecord.h"
#ifdef COUNT_TIME
#include "utilities/timer.h"
#endif

RRecord::RRecord(const std::string& str, const std::string & tag)
  : m_strFP(str)
{
  m_PT.put("name", tag);
}

RRecord::~RRecord()
{
#ifdef COUNT_TIME
  auto r = ff::timer_instance().get_durations();
  for(auto it=r.begin(); it != r.end(); ++it)
  {
    m_PT.put(it->first, it->second.count());
  }
#endif
  boost::property_tree::write_json(m_strFP, m_PT);
}


