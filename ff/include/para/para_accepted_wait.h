template<class PT, class WT>
class para_accepted_wait
{
    para_accepted_wait & operator = (const para_accepted_wait<PT, WT> &) = delete;
public:
	typedef typename PT::ret_type ret_type;
    para_accepted_wait(const para_accepted_wait<PT, WT> &) = default;
    para_accepted_wait(PT & p, const WT & w)
        : m_refP(p)	
		, m_oWaiting(w){}

    template<class F>
    auto		operator ()(F && f) -> internal::para_accepted_call<PT, ret_type>
    {
		typedef typename std::remove_reference<WT>::type WT_t;
		internal::para_impl_ptr<ret_type> pImpl = internal::make_para_impl<ret_type>(f);
		m_refP.m_pImpl = pImpl;
		internal::para_impl_wait_ptr<WT_t> pTask = std::make_shared<internal::para_impl_wait<WT_t> >(m_oWaiting, m_refP.m_pImpl);
		internal::schedule(pTask);
        return internal::para_accepted_call<PT, ret_type>(m_refP);
    }
protected:
    PT & m_refP;
	WT	m_oWaiting;
};//end class para_accepted_wait;