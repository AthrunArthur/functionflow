#include "ff.h"

using namespace std;
using namespace ff;

extern int sum;
extern int N;
extern bool place(int * x, int k);

accumulator<int> psum(0, [](const int & x, const int& y) {
    return x + y;
    });


void queen_parallel(int* x,int t)
{
  if(t>N && N>0)
  {
    psum.increase(1);
  }
  else
    for(int i=1; i<=N; i++)
    {
      x[t] = i; //The t-th queen is in colomn i.
      if(place(x,t))
        queen_parallel(x,t+1);
    }

}

int queen_para(const int* x,int t) {

  int xx[20];
  for(int k=0; k<20; k++)xx[k]=x[k];
  if(t>(N-8>0?N-8:N)&&N>0) {
    queen_parallel(xx,t);
  }
  else {
    ff::paracontainer pp;
    for(int i=1; i<=N; i++)
    {

      xx[t]=i;
      if(place(xx,t))
      {
        ff::para<void> p;
        p([xx,t]() {
            queen_para(xx,t+1);
            });
        pp.add(p);
      }
    }
    ff_wait(all(pp));
  }
  return 0;
}

