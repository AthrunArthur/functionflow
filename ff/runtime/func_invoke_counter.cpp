#include "utilities/func_invoke_counter.h"
#include <sstream>
#include <string>
#include <iostream>

namespace ff
{
thread_local_var<func_invoke_counter::counters_t> func_invoke_counter::m_oCounters;

void func_invoke_counter::init(int thrd_num){
m_oCounters = thread_local_var<counters_t>(thrd_num);
std::cout<<"func_invoke_counter::init "<<thrd_num<<std::endl;
for(int i = 0; i < m_oCounters.all().size();++i)
{
for(int j = 0; j < func_counter; ++j)
{
m_oCounters.all()[i].push_back(j);
}
}
}

void func_invoke_counter::call(int tag)
{
  m_oCounters.get()[tag] ++;
}

bool func_invoke_counter::is_normal()
{
return true;
}

std::string func_invoke_counter::status()
{
  std::stringstream ss;
  auto ts = func_invoke_counter::tags();
  for(auto it = ts.begin(); it != ts.end(); ++it)
  {
    for(size_t i = 0; i < m_oCounters.all().size(); ++i)
    {
      ss<<i<<" : "<<it->second<<":"<<m_oCounters.all()[i][it->first]<<"\n";
    }
  }
  return ss.str();
}
std::map<int, std::string> func_invoke_counter::tags()
{
  std::string ts[] = {
#define FUNC_TAG(a) #a ,
#include "utilities/func_tags.h"
#undef FUNC_TAG
    };
  std::map<int, std::string> res ;
  for(int i = 0; i < func_counter;++i)
  {
    res.insert(std::make_pair(i, ts[i]));
  }
  return res;
}

}//end namespace ff
