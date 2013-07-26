#include "common/common.h"

namespace ff{

exe_state operator &&(exe_state e1, exe_state e2)
{
	if(e1 == e2)
		return e1;
	return exe_state::exe_unknown;
}

exe_state operator ||(exe_state e1, exe_state e2)
{
	if(e1 == exe_state::exe_over ||
		e2 == exe_state::exe_over)
		return exe_state::exe_over;
	return exe_state::exe_unknown;
}
}//end namespace ff
