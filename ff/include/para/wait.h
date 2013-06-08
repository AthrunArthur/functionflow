#ifndef FF_PARA_WAIT_H_
#define FF_PARA_WAIT_H_
#include "common/common.h"

namespace ff{

class para;
	
	class paragroup;
	namespace internal
	{
	template <class T1, class T2>
	class wait_and
	{
	public:
		wait_and(T1 & t1, T2 & t2){}
		wait_and(T1 && t1, T2 && t2){}
		
	protected:
		
	};//end class wait_and
	
	template <class T1, class T2>
	class wait_or
	{
	public:
		wait_or(T1 & t1, T2 & t2){}
		wait_or(T1 && t1, T2 && t2){}
	};//end class wait_or
	
	class wait_all
	{
	public:
		wait_all(paragroup & pg){}
		wait_all(paragroup && pg){}
	};//end class wait_all
	
	class wait_any
	{
	public:
		wait_any(paragroup & pg){}
		wait_any(paragroup && pg){}
	};//end class wait_any
	
	
	}//end namespace internal
	
	template<class T1, class T2>
	internal::wait_and<T1, T2>&& operator &&(T1 & t1, T2 & t2)
	{
		return internal::wait_and<T1, T2>(t1, t2);
	}
	template<class T1, class T2>
	internal::wait_and<T1, T2>&& operator &&(T1 && t1, T2 && t2)
	{
		return internal::wait_and<T1, T2>(t1, t2);
	}
	template<class T1, class T2>
	internal::wait_or<T1, T2>&& operator ||(T1 & t1, T2 & t2)
	{
		return internal::wait_or<T1, T2>(t1, t2);
	}
	template<class T1, class T2>
	internal::wait_or<T1, T2>&& operator ||(T1 && t1, T2 && t2)
	{
		return internal::wait_or<T1, T2>(t1, t2);
	}
	
	internal::wait_all all(paragroup & pg)
	{
		return internal::wait_all(pg);
	}
	internal::wait_all all(paragroup &&pg)
	{
		return internal::wait_all(pg);
	}
	internal::wait_any any(paragroup & pg)
	{
		return internal::wait_any(pg);
	}
	internal::wait_any any(paragroup && pg)
	{
		return internal::wait_any(pg);
	}
}//end namespace ff

#endif