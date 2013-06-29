#ifndef FF_RUNTIME_ENV_H_
#define FF_RUNTIME_ENV_H_
#include "common/common.h"
#include "common/fsetjmp.h"
#include <memory>
#include <vector>
#include <functional>

namespace ff {
namespace rt {
class environment
{
protected:
	environment() = default;
	environment(const environment &) = delete;
	environment & operator = (const environment &) = delete;
public:
	static std::shared_ptr<environment> &	instance();
    void		init();

    inline int			get_thrd_num() const {return m_iThreadNum;}
protected:
    int			m_iThreadNum;
	static std::shared_ptr<environment> 	s_pInstance;
};//end class environment


class RTThreadInfo
{
protected:
	RTThreadInfo()
	: m_oEntryPoint(::ff::make_shared_jmp_buf())
	, m_oToExeCtxs(){}
public:
	typedef std::vector<std::tuple<::ff::jmp_buf_ptr, std::function<bool ()> > ctx_predicate;
	
	static std::shared_ptr<RTThreadInfo> instance();
	
	::ff::jmp_buf_ptr get_entry_point() const {return m_oEntryPoint;}
	
	ctx_predicate & get_to_exe_ctxs() {return m_oToExeCtxs;}
	
protected:
	thread_local static std::shared_ptr<RTThreadInfo> s_pInstance;
	::ff::jmp_buf_ptr m_oEntryPoint;
	
	ctx_predicate m_oToExeCtxs;
};//end class RTThreadInfo


}//end namespace rt
};//end namespace ff


#endif