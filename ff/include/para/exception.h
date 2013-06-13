#ifndef FF_PARA_EXECEPTION_H_
#define FF_PARA_EXECEPTION_H_
#include "common/common.h"
#include <exception>

namespace ff
{
	class used_para_exception : public std::exception
	{
	public:
		used_para_exception()
			: std::exception()
			, msg("Fatal error! Can't use a para or paragroup more than one time!"){}
		virtual const char * what() const throw()
		{
			return msg;
		}
	protected:
		const char * msg;
	};//end class used_para_exception
}//end namespace ff;

#endif
