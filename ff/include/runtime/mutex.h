#ifndef FF_RUNTIME_MUTEX_H_
#define FF_RUNTIME_MUTEX_H_

namespace ff{
	
	class mutex
	{
	public:
		mutex(){}
		mutex(const mutex &) = delete;
		mutex & operator = (const mutex & ) = delete;
		
		void		lock(){}
		void		unlock(){}
		
	protected:
	};//end class mutex
}//end namespace ff

#endif