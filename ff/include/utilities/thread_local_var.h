#ifndef FF_UTILITIES_THREAD_LOCAL_VAR_H_
#define FF_UTILITIES_THREAD_LOCAL_VAR_H_
#include "common/common.h"
#include <vector>
#include "runtime/env.h"

namespace ff{
  template<typename T>
  class thread_local_var
  {
    public:
      thread_local_var(int thrd_num)
      {
        for(int i = 0; i < thrd_num; ++i)
          m_oVars.push_back(T());
      }
      inline T& get(){
        thread_local static thrd_id_t id = ff::rt::get_thrd_id();
        return m_oVars[id];}

      inline std::vector<T> & all(){
      return m_oVars;}

    protected:
      std::vector<T>  m_oVars;

  };//end class thread_local_var
}//end namespace ff


#endif
