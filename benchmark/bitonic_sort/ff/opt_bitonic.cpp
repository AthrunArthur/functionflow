#include "ff.h"
using namespace ff;


extern void comparator(int *data, int s, int e, bool dir);


void bitonic_sort(int *data, int s, int e, bool up)
{
  int len = e-s;
  int max_k = static_cast<int>(log2(len));

  typedef std::pair<int, int> PT;
  typedef ff::rt::simo_queue<PT, 8> MQ_t;
  MQ_t buf_queue;
  std::atomic_bool is_stopped(false);
  auto con_size  = ff::rt::rt_concurrency();

  for(int k = 0; k < max_k; k++)
  {
    ff::paracontainer pc;
    is_stopped = false;
    int mask = 1<<k;
    int ns = len / (2*mask);

    for(int i = 0; i < con_size; ++i)
    {
      ff::para<> a;
      a([data, mask](){
          PT t;
          while(!is_stopped || buf_queue.size() != 0)
          {
          if(buf_queue.pop(t)){
          int n = t.first;
          int j = t.second;
          int pos = n*mask + j;
          comparator(data, pos, pos + mask, n%2 == up);
          }}});
      pc.add(a);
    }


    for(int n = 0; n < ns; ++n)
    {
      for(int i = 0; i < mask; ++i)
      {
       if(!buf_queue.push(i)){ 
        int pos = n*mask + i;
        comparator(data, pos, pos + mask, n%2 == up);
       }
      }
    }
    is_stopped = true;
    ff_wait(all(pc));
  }
}
