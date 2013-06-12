#ifndef FF_RUNTIME_ENV_H_
#define FF_RUNTIME_ENV_H_
#include <memory>
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
}//end namespace rt
};//end namespace ff


#endif