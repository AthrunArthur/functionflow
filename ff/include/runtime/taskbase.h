#ifndef FF_RUNTIME_TASK_BASE_H_
#define FF_RUNTIME_TASK_BASE_H_
#include <mutex>
#include <vector>
#include <memory>

namespace ff {
namespace rt {
class task_base {
public:
    enum TKind {
        user_t,
        context_t,
        end_t
    };
public:
	virtual ~task_base(){}
    task_base(TKind tk, bool rt_del_mem = false): m_iTKind(tk)
	, m_bRTDelThis(rt_del_mem){};

    virtual void	run() = 0;

    inline TKind		getTK() {
        return m_iTKind;
    }

	inline bool			isDelByRT(){return m_bRTDelThis;}
protected:
    TKind 	m_iTKind;
	bool	m_bRTDelThis;
};//end class task_base;
typedef task_base *  task_base_ptr;

}//end namespace rt
}//end namespace ff

#endif
