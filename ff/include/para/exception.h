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
			: std::exception("Error! A para or paragroup can be called only once.\n"){}
	};//end class used_para_exception
}//end namespace ff;

#endif