#ifndef FF_SCOPE_GUARD_H_
#define FF_SCOPE_GUARD_H_
//This is only for C++11 as we need lambda here!
#include <functional>

namespace ff {
	
class scope_guard {
public:
    scope_guard(const scope_guard & s) = delete;
    const scope_guard & operator = (const scope_guard & ) = delete;

    template<class F1, class F2>
    scope_guard(const F1 & ctor, const F2 & dtor)
        : f(dtor) {
        ctor();
    }
    ~scope_guard() {
        f();
    }
private:
    std::function<void(void)> f;
	
};//end class scope_guard

}//end namespace ff
#endif