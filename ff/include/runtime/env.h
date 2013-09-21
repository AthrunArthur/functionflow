#ifndef FF_RUNTIME_ENV_H_
#define FF_RUNTIME_ENV_H_
#include "common/common.h"
#include "common/fsetjmp.h"
#include <memory>
#include <vector>
#include <functional>

namespace ff {
namespace rt {
typedef int32_t thrd_id_t;

inline size_t  hardware_concurrency(){
	static int t = std::thread::hardware_concurrency() - 1;
	return t;
}
inline size_t rt_concurrency()
{
	return hardware_concurrency();
}

thrd_id_t get_thrd_id();

void set_local_thrd_id(thrd_id_t i);

  struct ctx_pdict{
    ::ff::jmp_buf_ptr ctx;
    std::function<bool ()> pdict;
  };
  typedef ctx_pdict * ctx_pdict_ptr;
  
  template <class F>
ctx_pdict_ptr make_ctx_pdict_ptr(  F && f)
{
  ctx_pdict_ptr p = new ctx_pdict();
  p->ctx = make_shared_jmp_buf();
  p->pdict = f;//std::bind([f](){f();});
  return p;
}

inline void free_ctx_pdict_ptr(ctx_pdict_ptr p)
{
  delete p;
}


}//end namespace rt
}//end namespace ff


#endif
