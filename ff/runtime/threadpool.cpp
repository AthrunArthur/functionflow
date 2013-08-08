#include "runtime/threadpool.h"
namespace ff {
namespace rt {

threadpool::threadpool()
    : m_oThreads() {}

void threadpool::join()
{
    for(auto t : m_oThreads)
    {
        t.join();
    }
}
}//end namespace rt
}//end namespace ff