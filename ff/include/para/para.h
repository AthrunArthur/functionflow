#ifndef FF_PARA_PAEA_H_
#define FF_PARA_PAEA_H_
#include "common/common.h"

namespace ff{
	class para{
		para(const para &) = delete;
		para & operator =(const para &) = delete;
	public:
		para(){};
		
		template <class WT>
		para & operator[](const WT & cond)
		{
			return * this;
		}
		template<class RT>
		para & operator() (std::function<RT ()> & f)
		{
			f();
			return * this;
		}
		template<class Func_t>
		para & operator() (Func_t f)
		{
			f();
			return * this;
		}
		
	};//end class para;
}//end namespace ff
#endif
