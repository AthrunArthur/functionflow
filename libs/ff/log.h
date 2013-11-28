/*
 The MIT License (MIT)
Copyright (c) 2013 <AthrunArthur>
Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
*/

#ifndef FF_COMMON_LOG_H_
#define FF_COMMON_LOG_H_
#include "ff/log/logger.h"
#include "ff/log/logwriter.h"
#include "ff/singlton.h"

#define USING_FF_LOG 1
namespace ff
{
enum LogLevel
{
    TRACE,
    DEBUG,
    INFO,
    WARN,
    ERROR,
    FATAL,
    NUM_LOG_LEVELS,
};

template<class T = LogLevel>
class fflog
{
public:
    inline static void		init(const T & l, const char * logfile)
    {
		ll = l;
		singleton<internal::logwriter<blocking_queue<std::string> > >::instance().run(logfile);
    }
    inline static void		init(const T & l, const std::string & logfile)
    {
		ll = l;
		singleton<internal::logwriter<blocking_queue<std::string> > >::instance().run(logfile.c_str());
    }
    static T	ll;
};//end class fflog

template<class T>
T fflog<T>::ll = ERROR;

namespace llog
{
	template<class T>
	struct enable_traits{
		static const bool value = false;
	};//end class enable_traits;
}
}//end namespace ff


#define DEF_LOG_MODULE(module) struct log_ ##module{};

#define ENABLE_LOG_MODULE(module) \
	namespace ff{namespace llog{ \
  template<> struct enable_traits<log_ ##module> { \
	static const bool value = true; };  \
	}}

#define LOG_LEVEL(level, module) if(::ff::fflog<>::ll <= level) \
	ff::internal::logger<ff::llog::enable_traits<log_ ## module>::value>() \
	<<"\t"<<#level<<"\t"<<#module<<"\t"<<__FILE__<<":"<<__LINE__<<":"<<__FUNCTION__<<"\t"
	
#define LOG_TRACE(module) LOG_LEVEL(ff::TRACE,module)
#define LOG_DEBUG(module) LOG_LEVEL(ff::DEBUG, module)
#define LOG_INFO(module) LOG_LEVEL(ff::INFO, module)
#define LOG_WARN(module) LOG_LEVEL(ff::WARN, module)
#define LOG_ERROR(module)  LOG_LEVEL(ff::ERROR, module)
#define LOG_FATAL(module) LOG_LEVEL(ff::FATAL, module)
#endif