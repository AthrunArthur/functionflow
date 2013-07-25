#ifndef FF_RNTIME_RING_BUFF_H_
#define FF_RNTIME_RING_BUFF_H_
#include "common/common.h"

namespace ff{
	namespace internal{
		
		//N should be power of 2, for example 256
		
		template <size_t N>
		class seq_num{
		public:
			seq_num(int n = 0)
			: m_num(n){}
			seq_num<N>(const seq_num<N> & n)
			: m_num(n.m_num.load()){}
			seq_num<N>(seq_num<N> && n)
			: m_num(n.m_num.load()){}
			
			seq_num<N>& increment()
			{
				m_num ++;
				if(m_num == N)
					m_num = 0;
				return *this;
			}
			
			seq_num<N>& decrement()
			{
				m_num --;
				if(m_num <0)
					m_num = N - 1;
				return *this;
			}
			
			seq_num<N> operator +(size_t t)
			{
				return seq_num<N>((m_num.load() + t)%N);
			}
			
			seq_num<N> & set(size_t t)
			{
				m_num = t%N;
				return *this;
			}
			
			seq_num<N> & operator -(size_t t)
			{
				return seq_num<N>((m_num.load() - t)%N);
				return *this;
			}
			
			bool operator ==(const seq_num<N> & n) const
			{
				return m_num.load() == n.m_num.load();
			}
			std::atomic<int> &	num(){return m_num;}
			const std::atomic<int> & num()const {return m_num;}
		protected:
			std::atomic<int> m_num;
			char _pad[CACHE_LINE_SIZE -sizeof(std::atomic<int>)];
		};//end class seq_num;
		
		template <class Ty_, size_t N>
		class ring_buff{
		public:
			const static size_t SIZE = N;
			ring_buff()
			: front(0)
			, back(0){}
			
			template<size_t NN>
			void		copy_from(ring_buff<Ty_, NN> * pt)
			{
				while(!is_full() && !pt->is_empty())
				{
					Ty_ t;
					pt->pop_front(t);
					push_front(t);
				}
			}
			bool		is_empty() const
			{
				return back == front;
			}
			bool		is_full()
			{
				return (back +1 == front);
			}
			
			bool 		push_front(const Ty_ & t)
			{
				if(is_full())
					return false;
				buf[front.num().load()] = t;
				front.increment();
				return true;
			}
			bool		pop_front(Ty_ & t)
			{
				if(is_empty())
					return false;
				front.decrement();
				t = buf[front.num().load()];
				
				return true;
			}
			bool		pop_back(Ty_& t)
			{
				if(is_empty())
					return false;
				t = buf[back.num().load()];
				back.increment();
				return true;
			}
			
			size_t  size()const
			{
				return (front.num() - back.num())%N;
			}
		protected:
			seq_num<N>	front;
			seq_num<N>	back;
			Ty_		buf[N];
		};//class ring_buff
	}//end namespace internal
}//end namespace ff
#endif