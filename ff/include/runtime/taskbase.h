#ifndef FF_RUNTIME_TASK_BASE_H_
#define FF_RUNTIME_TASK_BASE_H_
#include <mutex>
#include <vector>
#include <memory>

namespace ff {
namespace rt {
class task_base {
public:
	enum TKind{
		user_t,
		context_t,
		end_t
	};
public:
	task_base(TKind tk);
	
	virtual void	run(const std::thread::id & id) = 0;
	
	inline TKind		getTK(){return m_iTKind;}
protected:
	TKind 	m_iTKind;
};//end class task_base;
typedef std::shared_ptr<task_base> task_base_ptr;

}//end namespace rt
}//end namespace ff

#endif