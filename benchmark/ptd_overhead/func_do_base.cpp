#include "ff.h"

extern int func_gen();
extern void func_handle(int n);

using namespace ff;
/*
void func_do(int num)
{
  paracontainer pc;
  int blocksize = 1<<6;
  for(int i = 0; i < num / blocksize; ++i)
  {
    para<> a;
    a([i, blocksize](){
        for(int j = i; j < i + blocksize; j++)
        {
        auto t = func_gen();
        func_handle(t);
        }
        });
    pc.add(a);
  }
  ff_wait(all(pc));
}
*/

void single_do()
{
  para<int> a;
  a([]()->int{
        auto t = func_gen();
        func_handle(t);
      });
  ff_wait(a);
}
void func_do(int num)
{
  paracontainer pc;
  for(int i = 0; i < num; ++i)
  {
    para<> a;
    a([](){
        single_do();
        });
    pc.add(a);
  }
  ff_wait(all(pc));
}
