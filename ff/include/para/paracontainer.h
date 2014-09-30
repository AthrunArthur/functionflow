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
#ifndef FF_PARA_PARA_CONTAINER_H_
#define FF_PARA_PARA_CONTAINER_H_
#include "common/common.h"
#include "para/para.h"
#include "para/para_helper.h"
#include "para/paras_with_lock.h"
#include "common/log.h"
#include "runtime/env.h"
#include <cmath>

namespace ff{

    namespace internal {
        class wait_all;
        class wait_any;
        
    }//end namespace internal

    class paracontainer {
    public:
        typedef void ret_type;
    public:
        para<void> &  operator [](int index)
        {
            _DEBUG(
                    if(!m_pEntities)
                    {
                        LOG_FATAL(para)<<" m_pEntities is null";
                    }
            )
            return (*m_pEntities).entities[index];
        }
        size_t 	size() const
        {
            if(m_pEntities)
                return m_pEntities->entities.size();
            return 0;
        }
        ~paracontainer()
        {
        }

        //! Not thread safe!
        void add(const para< void >&  p)
        {
            add_impl(p);
        }

        void clear()
        {
            m_pEntities.reset();
        }
    protected:
        void add_impl(const para<void> & p)
        {
            if(!m_pEntities)
                m_pEntities = std::make_shared<internal::paras_with_lock>();
            m_pEntities->entities.push_back(p);
        }

        typedef std::shared_ptr<internal::paras_with_lock > Entities_t;

        friend internal::wait_all all(paracontainer & pg);
        friend internal::wait_any any(paracontainer & pg);
        std::shared_ptr<internal::paras_with_lock> & all_entities() {
            return m_pEntities;
        };

        std::shared_ptr<internal::paras_with_lock >	m_pEntities;
    };//end class paracontainer

}//end namespace ff

#endif
