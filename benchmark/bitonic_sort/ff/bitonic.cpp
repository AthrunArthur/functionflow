#include "ff.h"
using namespace ff;


extern void comparator(int *data, int s, int e, bool dir);

void bitonic_sort(int *data, int s, int e, bool up)
{
  int len = e-s;
  int max_k = static_cast<int>(log2(len));
  for(int k = 0; k < max_k; k++)
  {
    ff::paracontainer pc;
    int mask = 1<<k;
    int ns = len / (2*mask);
    for(int n = 0; n < ns; ++n)
    {
      for(int i = 0; i < mask; ++i)
      {
        para<> a;
        a([up, n, mask, i, data](){
        int pos = n*mask + i;
        comparator(data, pos, pos + mask, n%2 == up);
        });
        pc.add(a);
      }
    }
    ff_wait(all(pc));
  }
}
