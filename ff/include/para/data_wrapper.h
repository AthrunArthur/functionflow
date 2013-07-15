#ifndef FF_PARA_DATA_WRAPPER_H_
#define FF_PARA_DATA_WRAPPER_H_
#include "common/common.h"
#include <mutex>
#include <vector>

namespace ff
{
	template<class T>
	class accumulator{
	public:
		accumulator(const accumulator<T> & ) = delete;
		accumulator<T> & operator = (const accumulator<T> &) = delete;
	public:
		typedef std::function<T (const T&, const T&)> Functor_t;
		template<class FT>
		accumulator(T && value, FT && functor)
		: m_oValue(std::move(value))
		, Functor(std::move(functor)){}
		
		template<class FT>
		accumulator(FT && functor)
		: m_oValue()
		, Functor(std::move(functor)){}
		
		template<class TT>
		accumulator<T>& increase(TT && value){
			if(plocal == nullptr)
			{
				plocal = new T();
				m_oMutex.lock();
				m_pAllValues.push_back(plocal);
				m_oMutex.unlock();
			}
		  *plocal = std::move(Functor(*plocal, std::forward<TT>(value)));
		  return *this;
		}
		
		T & get(){
			for(T * p : m_pAllValues)
			{
				m_oValue = std::move(Functor(m_oValue, *p));
				delete p;
			}
			return m_oValue;
		}
	protected:
		T m_oValue;
		Functor_t	Functor;
		static thread_local T * plocal;
		std::vector<T *>	m_pAllValues;
		std::mutex			m_oMutex;
	};//end class accumulator
	
	template <class T>
	thread_local T * accumulator<T>::plocal(nullptr);
	
	template< class T>
	class single_assign
	{
		single_assign(const single_assign<T> & ) = delete;
		single_assign<T> & operator =(const single_assign<T> &) = delete;
	public:
		single_assign()
		: m_oValue()
		, m_bIsAssigned(false){}
		single_assign(const T & v)
		: m_oValue(v)
		, m_bIsAssigned(true){}
		
		single_assign<T> & operator =(const T & v)
		{
			if(m_bIsAssigned)
				return *this;
			m_bIsAssigned = true;
			m_oValue = v;
			return *this;
		}
		
		T & get() {return m_oValue;}
	protected:
		T m_oValue;
		std::atomic<bool> m_bIsAssigned;
	};//end class single_assign
}//end namespace ff;
#endif