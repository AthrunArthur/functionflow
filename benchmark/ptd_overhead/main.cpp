#include <iostream>
#include "utils.h"
#include <cstdlib>
#include <vector>

using namespace std;

const static int MAX_NUM = 1000000;
int func_gen()
{
  return rand()%MAX_NUM;
}

std::vector<int> primes;
bool is_prime(int n)
{
  if(n == 1 || n == 0)
    return false;
  if(n == 2)
    return true;
  int k = 2;
  for(int p : primes)
  {
    if(p >= sqrt(n) + 1)
      return true;
    if(n%p == 0)
      return false;
    k = p;
  }
  std::cout<<"how can this be, k : "<<k<<std::endl;
}
void func_handle(int n)
{
  is_prime(n);
}

void init_primes()
{
  for(int i = 2; i < MAX_NUM; i++)
  {
    if(is_prime(i))
    {
      primes.push_back(i);
    }
  }
}

extern void func_do(int num);


const static int NUM= 1024;
int main(int argc, char *argv[])
{
  RRecord rr("time.json", "ptd");
  ParamParser pp;
  pp.parse(argc, argv);
  int thrd_num = pp.get<int>("thrd-num");
  rr.put("thrd-num", thrd_num);
  bool bIsPara = (thrd_num > 0);
  if(thrd_num > 0)
    initial_para_env(thrd_num);

  init_primes();
  std::cout<<" init "<<primes.size()<<" primes!"<<std::endl;

  start_record_cache_access();
  rr.run("elapsed-time", func_do, NUM);
  end_record_cache_access(rr);
}
