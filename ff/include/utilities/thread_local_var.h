#ifndef FF_UTILITIES_THREAD_LOCAL_VAR_H_
#define FF_UTILITIES_THREAD_LOCAL_VAR_H_
#include "common/common.h"
#include <vector>
#include "runtime/rtcmn.h"

namespace ff{
  template<typename T>
  class thread_local_var
  {
    public:
      thread_local_var(){
        for(size_t i = 0; i < rt::concurrency(); ++i){
          m_oVars.push_back(T());
        }
      }
      thread_local_var(const T& t){
        for(size_t i = 0; i < rt::concurrency(); ++i){
          m_oVars.push_back(T());
        }
      }

      inline T& current(){
        thread_local static thrd_id_t id = ff::rt::get_thrd_id();
        return m_oVars[id];
      }

      T get(){
        T res();
        for(size_t i = 0; i < m_oVars.size(); ++i)
          res += m_oVars[i];
        return res;
      }

      template<typename F>
      T get(F && f){
      }

    protected:
      std::vector<T>  m_oVars;

  };//end class thread_local_var
}//end namespace ff


#endif
