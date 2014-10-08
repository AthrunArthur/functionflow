/***********************************************
The MIT License (MIT)

Copyright (c) 2012 Athrun Arthur <athrunarthur@gmail.com>

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
*************************************************/
#ifndef FF_PARA_DATA_WRAPPER_H_
#define FF_PARA_DATA_WRAPPER_H_
#include "common/common.h"
#include "runtime/env.h"
#include <mutex>
#include <vector>

namespace ff
{
template<class T>
class accumulator {
public:
    accumulator(const accumulator<T> & ) = delete;
    accumulator<T> & operator = (const accumulator<T> &) = delete;
public:
    typedef std::function<T (const T&, const T&)> Functor_t;
    template<class FT>
    accumulator(const T & value, FT && functor)
        : m_oValue(std::move(value))
        , Functor(std::move(functor)) {
        for(int i = 0; i < ::ff::rt::rt_concurrency(); ++i)
        {
            m_pAllValues.push_back(new T(value));
        }
    }

    template<class FT>
    accumulator(FT && functor)
        : m_oValue()
        , Functor(std::move(functor)) {
        for(int i = 0; i < ::ff::rt::rt_concurrency(); ++i)
        {
            m_pAllValues.push_back(new T());
        }
    }

    ~accumulator(){
        for(int i = 0; i < m_pAllValues.size(); ++i)
          delete m_pAllValues[i];
    }

    template<class TT>
    accumulator<T>& increase(const TT & value) {
        thrd_id_t id = ff::rt::get_thrd_id();
        T * plocal = m_pAllValues[id];
        *plocal = std::move(Functor(*plocal, value));
        return *this;
    }

    T  get() {
        T v = m_oValue;
        for(T * p : m_pAllValues)
        {
            v = std::move(Functor(v, *p));
        }
        return v;
    }
protected:
    T m_oValue;
    Functor_t	Functor;
    std::vector<T *>	m_pAllValues;
    std::mutex			m_oMutex;
};//end class accumulator


template< class T>
class single_assign
{
    single_assign(const single_assign<T> & ) = delete;
    single_assign<T> & operator =(const single_assign<T> &) = delete;
public:
    single_assign()
        : m_oValue()
        , m_bIsAssigned(false) {}
    single_assign(const T & v)
        : m_oValue(v)
        , m_bIsAssigned(true) {}

    single_assign<T> & operator =(const T & v)
    {
        if(m_bIsAssigned)
            return *this;
        m_bIsAssigned = true;
        m_oValue = v;
        return *this;
    }

    T & get() {
        return m_oValue;
    }
protected:
    T m_oValue;
    std::atomic<bool> m_bIsAssigned;
};//end class single_assign
}//end namespace ff;
#endif
