#include "ff.h"
#include <vector>

extern int func_gen();
extern void func_handle(int n);

using namespace ff;
void single_do()
{
  para<int> a;
  a([]()->int{
      auto t = func_gen();
      });
  para<void> b;
  b[a]([](int r){
      func_handle(r);
      });
  ff_wait(a&&b);
}
void func_do(int num)
{
  paracontainer pc;
  for(int i = 0; i < num; ++i)
  {
    para<void> b;
    b([](){
        single_do();
        });
    pc.add(b);
  }
  ff_wait(all(pc));
}
