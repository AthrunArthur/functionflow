#ifndef COMMON_LOG_H_
#define COMMON_LOG_H_
//#define SYNC_WRITING_LOG

//#include "ff/log.h"




#ifdef USING_FF_LOG
#define _DEBUG(stmt) stmt;
#else
#define _DEBUG(stmt)
#define DEF_LOG_MODULE(m) 
#define ENABLE_LOG_MODULE(m)
#endif

DEF_LOG_MODULE(main)
DEF_LOG_MODULE(thread)
DEF_LOG_MODULE(para)
DEF_LOG_MODULE(rt)
DEF_LOG_MODULE(queue)

//ENABLE_LOG_MODULE(main)
ENABLE_LOG_MODULE(thread)
ENABLE_LOG_MODULE(para)
ENABLE_LOG_MODULE(rt)
ENABLE_LOG_MODULE(queue)
#endif
