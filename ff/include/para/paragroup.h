#ifndef FF_PARA_PARA_GROUND_H_
#define FF_PARA_PARA_GROUND_H_
#include "common/common.h"

namespace ff{
	class paragroup{
	public:
		template<class Iterator_t, class Functor_t>
		paragroup & for_each(Iterator_t begin, Iterator_t end, Functor_t f){
			Iterator_t t = begin;
			while(t!= end)
			{
				f(*t);
			}
			return *this;
		}
		
		template <class ITEM, class RT1, class RT2>
		std::function<void (ITEM &)> 
			pipeline(std::function<RT1 (ITEM &)> f1,
					 std::function<RT2 (RT1& )> f2)
		{
			return [](ITEM & item){ f2(f1(item));};
		}
	};//end class paragroup
	
}//end namespace ff

#endif 