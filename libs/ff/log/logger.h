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

#ifndef FF_LOG_LOGGER_H_
#define FF_LOG_LOGGER_H_
#include <string>
#include <cstring>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <exception>
#include "ff/log/logwriter.h"
#include "ff/singlton.h"

#ifdef SYNC_WRITING_LOG
#include <iostream>
#endif

#if __cplusplus < 201103L
#include <boost/date_time/posix_time/posix_time.hpp>
#define BOOST_DATE_TIME_SOURCE
#else
#include <chrono>
#include <thread>
#endif
namespace ff
{
namespace internal {
template<bool EnableLogFlag>
class logger {
    std::stringstream  buffer_;
public:
    virtual ~logger()
    {
        try {
	  std::stringstream ss;
#if __cplusplus < 201103L
	  std::string str = boost::posix_time::to_iso_string(boost::posix_time::microsec_clock::local_time());
	  ss<<str <<"\t"<<boost::this_thread::get_id()<<buffer_.str();
#else
            std::chrono::time_point<std::chrono::system_clock> now = std::chrono::system_clock::now();
            time_t now_c = std::chrono::system_clock::to_time_t(now);
            
            const char * s = asctime(std::localtime(&now_c));
	    std::string str(s, std::strlen(s) -1);
	    ss<<str<<"\t"<<std::this_thread::get_id()<<buffer_.str();
#endif

            
            
#ifdef SYNC_WRITING_LOG
	    ff::singleton<logwriter<> >::instance().flush(ss.str());
#else
            ff::singleton<logwriter<> >::instance().queue().push_back(ss.str());
#endif
        } catch(const std::exception & e)
        {
        }
    }
public:
    typedef logger<EnableLogFlag> self;

    template<typename T>
    self& operator<<(T v)
    {
        buffer_ << v;
        return * this;
    }
    template<typename T>
    self& operator<<(T * p)
    {
        uintptr_t v = reinterpret_cast<uintptr_t>(p);
        buffer_<<"0x"<<std::hex<<v<<"  ";
        return *this;
    }
    self & operator<<(const char * p) {
        buffer_<<p;
        return *this;
    }


    self & operator<<(bool v)
    {
        buffer_<<(v ? "1": "0");
        return *this;
    }

};

template<>
class logger<false> {
public:
    typedef logger<false> self;
    template<typename T>
    self& operator<<(T v) {
        return *this;
    }
    template<typename T>
    self& operator<< (T *v) {
        return *this;
    }
};
}//end namespace internal

}//end namespace ff
#endif