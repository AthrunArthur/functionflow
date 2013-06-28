//This will enable a better interface for jmp_buf
#ifndef FF_COMMON_FSETJMP_H_
#define FF_COMMON_FSETJMP_H_
#include <csetjmp>

namespace ff{
template<class T>
struct array_len {};

template<class T, size_t N>
struct array_len<T[N]> {
	static const size_t len = N;
};
typedef typename std::remove_extent<std::jmp_buf>::type jmp_buf_primitive;
typedef std::shared_ptr<jmp_buf_primitive> jmp_buf_ptr;


inline jmp_buf_ptr make_shared_jmp_buf()
{
	return jmp_buf_ptr (new jmp_buf_primitive[array_len<jmp_buf>::len], 
					[](jmp_buf_primitive *p){delete p;});
}

}//end namespace ff
#endif