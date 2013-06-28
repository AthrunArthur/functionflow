#include "runtime/env.h"
#include <thread>

namespace ff {
namespace rt {

std::shared_ptr<environment> environment::s_pInstance = nullptr;

void environment::init()
{
    m_iThreadNum = std::thread::hardware_concurrency();
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

std::shared_ptr<RTThreadInfo> RTThreadInfo::s_pInstance(nullptr);

std::shared_ptr<RTThreadInfo> RTThreadInfo::instance()
{
	if(!s_pInstance)
		s_pInstance = std::make_shared<RTThreadInfo>();
	return s_pInstance;
}
}//end namespace rt
}//end namespace ff