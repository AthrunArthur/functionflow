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
#ifndef FF_PARA_PAEA_H_
#define FF_PARA_PAEA_H_
#include "common/common.h"
#include "common/function_traits.h"
#include "para/para_helper.h"
#include "para/exception.h"
#include "para/para_impl.h"
#include "runtime/rtcmn.h"
#include "para/para_wait_traits.h"


namespace ff {

  using namespace ff::utils;
  namespace internal {
    template<typename DT, typename RT>
      class para_common {
        public:
          typedef RT  ret_type;
        public:
#include "para/para_accepted_wait.h"
          para_common()
            : m_pImpl(nullptr) {};
          ~para_common()
          {
          }
          template<class WT>
            auto operator[](WT && cond) ->
            typename std::enable_if<is_para_or_wait<typename std::remove_reference<WT>::type>::value, para_accepted_wait<DT, WT> >::type
            {
              if(cond.get_state() == exe_state::exe_empty)
                throw empty_para_exception();
              return para_accepted_wait<DT, WT>(*(static_cast<DT *>(this)),std::forward<WT>(cond));
            }
          template<class WT>
            auto operator[](WT && cond) ->
            typename std::enable_if<!is_para_or_wait<typename std::remove_reference<WT>::type>::value, para_accepted_wait<DT, para<void>> >::type
            {
              static_assert(Please_Check_The_Assert_Msg<WT>::value, FF_EM_WRONG_USE_SQPAREN);
            }
          template<class F>
            auto		exe(F && f) -> para_accepted_call<DT, ret_type>
            {
              if(m_pImpl)
                throw used_para_exception();
              m_pImpl = make_para_impl<ret_type>(std::forward<F>(f));
              schedule(m_pImpl);
              return para_accepted_call<DT, ret_type>(*(static_cast<DT *>(this)));
            }
          template<class F>
            auto		operator ()(F && f) ->
            typename std::enable_if<std::is_same<typename ::ff::utils::function_res_traits<F>::ret_type, ret_type>::value, para_accepted_call<DT, ret_type> >::type
            {
              return exe(std::forward<F>(f));
            }

          template<class F>
            auto operator()(F && f) ->
            typename std::enable_if<!std::is_same<typename ::ff::utils::function_res_traits<F>::ret_type, ret_type>::value, para_accepted_call<DT, ret_type> >::type
            {
              static_assert(Please_Check_The_Assert_Msg<F>::value, FF_EM_CALL_WITH_TYPE_MISMATCH);
            }
          exe_state	get_state()
          {
            if(m_pImpl)
              return m_pImpl->get_state();
            return exe_state::exe_empty;
          }
          bool	check_if_over()
          {
            if(m_pImpl)
              return m_pImpl->check_if_over();
            return false;
          }

          internal::para_impl_ptr<ret_type> get_internal_impl() {
            return m_pImpl;
          }

          template<class F>
            void     then(const F& f)
            {
              static_assert(Please_Check_The_Assert_Msg<F>::value, FF_EM_CALL_THEN_WITHOUT_CALL_PAREN);
            }
        protected:
          internal::para_impl_ptr<ret_type> m_pImpl;
      };//end class para_common

  }//end namespace internal

  template<typename RT = void>
    class para : public internal::para_common<para<RT>, RT > {
      public:
        auto get() -> typename std::enable_if< !std::is_void<RT>::value,RT>::type &
        {
          return internal::para_common<para<RT>,RT >::m_pImpl->get();
        }
    };//end class para;

  template<>
    class para<void> : public internal::para_common<para<void>, void > {
    };//end class para;

  template<class T>
    class para< para<T> > {
      public:
        para(){
          static_assert(Please_Check_The_Assert_Msg<para<T>>::value, FF_EM_CALL_NO_SUPPORT_FOR_PARA);
        };
    };//end class para;

}//end namespace ff
#endif
