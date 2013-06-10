#ifndef FF_PARA_PARA_HELPER_H_
#define FF_PARA_PARA_HELPER_H_
#include "common/function_traits.h"

namespace ff {
template<class RT> class para;
namespace internal {
using namespace ff::utils;
	
template <class T>
class para_ret {
public:
    para_ret(para<T> & p)
        : m_refP(p)
		, m_oValue(){}

    T & get() {
        return m_oValue;
    }
    void set(T& v) {
        m_oValue = v;
    }
    void set(T&& v){
		m_oValue = v;
	}

protected:
    para<T> &	m_refP;
    T m_oValue;
};//end class para_ret;

}//end namespace internal;
}//end namespace ff
#endif
