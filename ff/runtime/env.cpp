#include "runtime/env.h"
namespace ff {
namespace rt {

std::shared_ptr<environment> environment::s_pInstance = nullptr;

void environment::init()
{
    m_iThreadNum = 3;
}
std::shared_ptr<environment> & environment::instance()
{
	if(s_pInstance == nullptr)
	{
		s_pInstance = std::shared_ptr<environment>(new environment());
		s_pInstance->init();
	}
	return s_pInstance;
}
}//end namespace rt
}//end namespace ff