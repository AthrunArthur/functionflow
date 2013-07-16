#ifndef FF_H_
#define FF_H_

#include "common/common.h"
#include "para/data_wrapper.h"
#include "para/para.h"
#include "para/paragroup.h"
#include "para/wait.h"

namespace ff{

template<class W>
void ff_wait(W && wexpr)
{
	(wexpr).then([](){});
}//end wait
template<class RT>
void ff_wait(para<RT> & sexpr)
{
	ff_wait(sexpr && sexpr);
}

}//end namespace ff

#endif
