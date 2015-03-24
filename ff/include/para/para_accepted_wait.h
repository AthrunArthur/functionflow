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
template<class PT, class WT>
class para_accepted_wait
{
  para_accepted_wait & operator = (const para_accepted_wait<PT, WT> &) = delete;
  public:
  typedef typename PT::ret_type ret_type;
  typedef typename std::remove_reference<WT>::type WT_t;
  typedef typename WT_t::ret_type wret_type;

  para_accepted_wait(const para_accepted_wait<PT, WT> &) = default;
  para_accepted_wait(PT & p, const WT & w)
    : m_refP(p)
      , m_oWaiting(w){}

    template<class F>
    auto		operator ()(F && f) ->
    typename std::enable_if<utils::function_args_traits<F>::is_no_args, internal::para_accepted_call<PT, ret_type> >::type
    {
      internal::para_impl_ptr<ret_type> pImpl = internal::make_para_impl<ret_type>(f);
      m_refP.m_pImpl = pImpl;
      //internal::para_impl_wait_ptr<WT_t> pTask = new internal::para_impl_wait<WT_t>(m_oWaiting, m_refP.m_pImpl);
      internal::para_impl_wait_ptr<WT_t> pTask = std::make_shared<internal::para_impl_wait<WT_t> >(m_oWaiting, m_refP.m_pImpl);
      internal::schedule(pTask);
      return internal::para_accepted_call<PT, ret_type>(m_refP);
    }
    template<class F>
    auto       operator()(F && f) ->
    typename std::enable_if<!std::is_same<wret_type, void>::value &&
                  utils::is_function_with_arg_type<F, wret_type>::value,
    internal::para_accepted_call<PT, ret_type> >::type
    {
      internal::para_impl_ptr<ret_type> pImpl = internal::make_para_impl<ret_type>([this, &f](){
          auto t = m_oWaiting.get();//!TODO here could be segment fault!
          f(t);
          });
      m_refP.m_pImpl = pImpl;
      internal::para_impl_wait_ptr<WT_t> pTask = std::make_shared<internal::para_impl_wait<WT_t> >(m_oWaiting, m_refP.m_pImpl);
      internal::schedule(pTask);
      return internal::para_accepted_call<PT, ret_type>(m_refP);
    }
  template<class F>
    void        then(F && f) {
      static_assert(Please_Check_The_Assert_Msg<F>::value, FF_EM_CALL_THEN_WITHOUT_CALL_PAREN);
    }
  protected:
  PT & m_refP;
  WT	m_oWaiting;
};//end class para_accepted_wait;
