#ifndef FF_RUNTIME_ENV_H_
#define FF_RUNTIME_ENV_H_
#include "common/common.h"
#include "common/fsetjmp.h"
#include <memory>
#include <vector>
#include <functional>

namespace ff {
namespace rt {
  struct ctx_pdict{
    ::ff::jmp_buf_ptr ctx;
    std::function<bool ()> pdict;
  };
  
  /*
class RTThreadInfo
{
protected:
	RTThreadInfo()
	: m_oEntryPoint(::ff::make_shared_jmp_buf())
	, m_bIsMainThread(true)
	, m_oToExeCtxs(){}
public:
	typedef std::vector< ctx_pdict > ctx_predicate;
	
	static std::shared_ptr<RTThreadInfo> instance();
	
	::ff::jmp_buf_ptr get_entry_point() const {return m_oEntryPoint;}
	
	ctx_predicate & get_to_exe_ctxs() {return m_oToExeCtxs;}
	
	void	check_and_run_paused_ctx();
	
	void	erase_runned_ctx(::ff::jmp_buf_ptr ctx);
	
	void	clear_main_thread(){m_bIsMainThread = false;}
	
	bool	is_main_thread(){return m_bIsMainThread;}
protected:
	thread_local static std::shared_ptr<RTThreadInfo> s_pInstance;
	::ff::jmp_buf_ptr m_oEntryPoint;
	bool		m_bIsMainThread;
	ctx_predicate m_oToExeCtxs;
};//end class RTThreadInfo
*/

}//end namespace rt
}//end namespace ff


#endif
