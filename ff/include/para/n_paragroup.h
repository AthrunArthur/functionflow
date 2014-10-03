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
#ifndef FF_PARA_PARA_GROUND_H_
#define FF_PARA_PARA_GROUND_H_
#include "common/common.h"
#include "para/para.h"
#include "para/para_helper.h"
#include "para/paras_with_lock.h"
#include "common/log.h"
#include "common/any.h"
#include "runtime/env.h"

#include <cmath>
#include <algorithm>
#include <map>

namespace ff {

    namespace internal {
        class wait_all;
        class wait_any;
    }//end namespace internal
    struct auto_partitioner {};
    struct simple_partitioner {};

    class paragroup {
    public:
        typedef void ret_type;
    public:
        template<class PT, class WT>
        class para_accepted_wait
        {
            para_accepted_wait & operator = (const para_accepted_wait<PT, WT> &) = delete;
        public:
            typedef typename PT::ret_type ret_type;
            para_accepted_wait(const para_accepted_wait<PT, WT> &) = default;
            para_accepted_wait(PT & p, const WT & w)
                    : m_refP(p)
                    , m_oWaiting(w) {}


            //!For arithmetic iterators
            template<class Iterator_t, class Functor_t>
            auto for_each(Iterator_t begin, Iterator_t end, Functor_t && f)
            -> typename std::enable_if<
                    std::is_arithmetic<
                            typename std::remove_cv<Iterator_t>::type>::value,
                    internal::para_accepted_call<paragroup, void>>::type
            {
                init_iterator_range_map<Iterator_t>();
                FF_DEFAULT_PARTITIONER  *p = nullptr;
                for_each_impl(begin, end, std::forward<Functor_t>(f), m_refP.m_pEntities, p);
                //for_each_impl_general_iterator(begin, end, std::forward<Functor_t>(f), m_refP.m_pEntities, p);
                return internal::para_accepted_call<paragroup, ret_type>(m_refP);
            }

            //!For random-access iterators
            template<class Iterator_t, class Functor_t>
            auto for_each(Iterator_t begin, Iterator_t end, Functor_t && f)
            -> typename std::enable_if<
                    !std::is_arithmetic<typename std::remove_cv<Iterator_t>::type>::value &&
                            std::is_same<typename std::iterator_traits<Iterator_t>::iterator_category, std::random_access_iterator_tag>::value,
                    internal::para_accepted_call<paragroup, void>>::type
            {
                init_iterator_range_map<Iterator_t>();
                FF_DEFAULT_PARTITIONER  *p = nullptr;
                for_each_impl(begin, end, [f](const Iterator_t & t) {
                    //for_each_impl_general_iterator(begin, end, [f](const Iterator_t & t) {
                    f(*t);
                }, m_refP.m_pEntities, p);
                return internal::para_accepted_call<paragroup, ret_type>(m_refP);
            }

            //!For general iterators
            template<class Iterator_t, class Functor_t>
            auto for_each(Iterator_t begin, Iterator_t end, Functor_t && f)
            -> typename std::enable_if<
                    !std::is_arithmetic<typename std::remove_cv<Iterator_t>::type>::value &&
                            ! std::is_same<typename std::iterator_traits<Iterator_t>::iterator_category, std::random_access_iterator_tag>::value,
                    internal::para_accepted_call<paragroup, void>>::type
            {
                FF_DEFAULT_PARTITIONER  *p = nullptr;
                for_each_impl_general_iterator(begin, end, [f](const Iterator_t & t) {
                    f(*t);
                }, m_refP.m_pEntities, p);
                return internal::para_accepted_call<paragroup, ret_type>(m_refP);
            }


        protected:
            PT & m_refP;
            WT	m_oWaiting;
        };//end class para_accepted_wait;

        paragroup()
                : m_pParas(nullptr)
                , m_oPlaceHolder(){
            m_oPlaceHolder([](){});
        }
        ~paragroup(){}

        template <class WT>
        para_accepted_wait<paragroup, WT> operator[](WT && cond)
        {
            return para_accepted_wait<paragroup,WT>(*this, std::forward<WT>(cond));
        }
        //!For input index, return the corresponding task
        template <class Iterator_t>
        para<void> &  task(const Iterator_t & index)
        {
            iterator_range ir(index, index);
            Iterator2Para_t::iterator it = m_pParas->find(ir);
            if(it == m_pParas->end())
            {
              std::cout<<"fuck, how can this be!!"<<index<<std::endl;
              for(Iterator2Para_t::iterator kt = m_pParas->begin(); kt != m_pParas->end(); ++kt)
              {
                iterator_range kr = kt->first;
                  std::cout<<kr.begin.get<Iterator_t>()<<", "<<kr.end.get<Iterator_t>()<<std::endl;
              }
              assert(0 && "geeee");
            }
            return it->second;
        }

        size_t 	size() const
        {
            if(m_pEntities)
                return m_pEntities->entities.size();
            return 0;
        }

        //! For arithmetic iterator
        template<class Iterator_t, class Functor_t>
        auto for_each(Iterator_t begin, Iterator_t end, Functor_t && f)
        -> typename std::enable_if<
                std::is_arithmetic<
                        typename std::remove_cv<Iterator_t>::type>::value,
                internal::para_accepted_call<paragroup, void> >::type
        {
            init_iterator_range_map<Iterator_t>();
            FF_DEFAULT_PARTITIONER  *p = nullptr;
            for_each_impl(begin, end, std::forward<Functor_t>(f), m_pEntities, p);
            //for_each_impl_general_iterator(begin, end, std::forward<Functor_t>(f), m_pEntities, p);
            return internal::para_accepted_call<paragroup, ret_type>(*this);
        }

        //! For random-access iterator
        template<class Iterator_t, class Functor_t>
        auto for_each(Iterator_t begin, Iterator_t end, Functor_t && f)
        -> typename std::enable_if<
                !std::is_arithmetic<typename std::remove_cv<Iterator_t>::type>::value &&
                        std::is_same<typename std::iterator_traits<Iterator_t>::iterator_category, std::random_access_iterator_tag>::value,
                internal::para_accepted_call<paragroup, void>>::type
        {
            init_iterator_range_map<Iterator_t>();
            FF_DEFAULT_PARTITIONER  *p = nullptr;
            for_each_impl(begin, end, [f](const Iterator_t & t) {
                //for_each_impl_general_iterator(begin, end, [f](const Iterator_t & t) {
                f(*t);
            }, m_pEntities, p);
            return internal::para_accepted_call<paragroup, ret_type>(*this);
        }

        //! For general iterator
        template<class Iterator_t, class Functor_t>
        auto for_each(Iterator_t begin, Iterator_t end, Functor_t && f)
        -> typename std::enable_if<
                !std::is_arithmetic<typename std::remove_cv<Iterator_t>::type>::value &&
                        ! std::is_same<typename std::iterator_traits<Iterator_t>::iterator_category, std::random_access_iterator_tag>::value,
                internal::para_accepted_call<paragroup, void> >::type
        {
            FF_DEFAULT_PARTITIONER  *p = nullptr;
            for_each_impl_general_iterator(begin, end, [f](const Iterator_t & t) {
                f(*t);
            }, m_pEntities, p);
            return internal::para_accepted_call<paragroup, ret_type>(*this);
        }

        void clear()
        {
            m_pEntities.reset();
            m_pParas->clear();
        }
    protected:
        typedef std::shared_ptr<internal::paras_with_lock > Entities_t;

#include "para/n_paragroup_general_iterator_impl.h"

#include "para/n_paragroup_random_access_iterator.h"


    protected:
        friend internal::wait_all all(paragroup & pg);
        friend internal::wait_any any(paragroup & pg);
        std::shared_ptr<internal::paras_with_lock> & all_entities() {
            return m_pEntities;
        };

        template<class Iterator_t>
        void init_iterator_range_map()
        {
            m_pParas = new Iterator2Para_t(iterator_range_comparator<Iterator_t>::cmp);
        }

    protected:
        struct iterator_range
        {
            any_value begin;
            any_value end;
            template<class Iterator_t>
            iterator_range(const Iterator_t & b, const Iterator_t e)
                    : begin(b)
                    , end(e){};
        };
        struct default_iterator_range_comparator{
            static bool cmp(const iterator_range & a, const iterator_range & b)
            {
                return (&a < &b);
            };
        };
        template <class T>
        struct iterator_range_comparator{
            static bool cmp(const iterator_range & a, const iterator_range & b)
            {
                if(a.begin.get<T>() < b.begin.get<T>() && a.end.get<T>() <= b.begin.get<T>())
                    return true;
                return false;
            };
        };
        typedef std::map<iterator_range, para<void>,
            std::function<bool (const iterator_range & r1, const iterator_range & r2) > > Iterator2Para_t;

        Iterator2Para_t *m_pParas;
        ff::spinlock   m_oParasLock;

        ff::para<void>     m_oPlaceHolder;

        std::shared_ptr<internal::paras_with_lock >	m_pEntities;
    };//end class paragroup



}//end namespace ff
#endif
