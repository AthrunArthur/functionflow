#include "common/common.h"
#include "common/fsetjmp.h"

namespace ff{

exe_state operator &&(exe_state e1, exe_state e2)
{
	if(e1 == e2)
		return e1;
	return exe_state::exe_unknown;
}

exe_state operator ||(exe_state e1, exe_state e2)
{
	if(e1 == exe_state::exe_over ||
		e2 == exe_state::exe_over)
		return exe_state::exe_over;
	return exe_state::exe_unknown;
}


std::string dump_ctx(jmp_buf p)
{
  union char_decoder{
    char c;
    struct {
      uint16_t h1 : 4;
      uint16_t h2 : 4;
    } d;
  };
  
  std::stringstream ss;
  int len = sizeof(jmp_buf_primitive) * array_len<std::jmp_buf>::len;
  ss<<"0x"<<std::hex<<p<<"(" << len <<"): ";
  char_decoder cd;
  for(int i = 0; i < len; ++i)
  {
    const char * t = (const char *)(p);
    cd.c = *(t + i);
    ss<<std::hex<<cd.d.h1;
    ss<<std::hex<<cd.d.h2 << " ";
  }
  return ss.str();
}

}//end namespace ff
