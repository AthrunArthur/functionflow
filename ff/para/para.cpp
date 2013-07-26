#include "common/common.h"
#include "para/wait.h"

namespace ff{
namespace internal{

wait_all::wait_all(std::shared_ptr<std::vector<para<void> > > ps)
: all_ps(ps)
, m_iES(exe_state::exe_unknown){};

exe_state wait_all::get_state()
{
	if(m_iES != exe_state::exe_over)
	{
		m_iES = exe_state::exe_over;
		for(auto p = all_ps->begin(); p != all_ps->end();++p)
			m_iES = m_iES && p->get_state();
	}
	return m_iES;
}

bool wait_all::check_if_over()
{
	if(m_iES == exe_state::exe_over)
		return true;
	get_state();
	if(m_iES == exe_state::exe_over)
		return true;
	return false;
}

wait_any::wait_any(std::shared_ptr<std::vector<para<void> > > ps)
: all_ps(ps)
, m_iES(exe_state::exe_unknown){};


exe_state wait_any::get_state()
{
	if(m_iES != exe_state::exe_over)
	{
		m_iES = exe_state::exe_over;
		for(auto p = all_ps->begin(); p != all_ps->end();++p)
			m_iES = m_iES || p->get_state();
	}
	return m_iES;
}

bool wait_any::check_if_over()
{
	if(m_iES == exe_state::exe_over)
		return true;
	get_state();
	if(m_iES == exe_state::exe_over)
		return true;
	return false;
}
}//end namespace internal
internal::wait_all all(paragroup & pg)
{
	return internal::wait_all(pg.all_entities());
}
internal::wait_any any(paragroup & pg)
{
	return internal::wait_any(pg.all_entities());
}
}//end namespace ff
