#include "rrecord.h"

RRecord::RRecord(const std::string& str, const std::string & tag)
  : m_strFP(str)
{
  m_PT.put("name", tag);
}

RRecord::~RRecord()
{
  boost::property_tree::write_json(m_strFP, m_PT);
}
