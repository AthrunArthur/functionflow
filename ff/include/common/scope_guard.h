#ifndef FF_COMMON_SCOPE_GUARD_H_
#define FF_COMMON_SCOPE_GUARD_H_

#include <functional>

namespace ff
{
  class scope_guard{
  public:
    scope_guard(const scope_guard & s) = delete;
    const scope_guard & operator = (const scope_guard & ) = delete;
    
    template<class F1, class F2>
    scope_guard(const F1 & ctor, const F2 & dtor)
    : f(dtor){
      ctor();
    }
    ~scope_guard(){f();}
  private:
    std::function<void(void)> f;
  };//end class scope_guard
}//end namespace ff

#endif