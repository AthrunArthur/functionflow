#include <chrono>
#include <cstdio>
#include <stdio.h>
#include <sstream>
#include <cstdlib>
#include <cassert>
#include <fstream>
#include <iostream>
#include <utility>
#include <assert.h>
#include "utils.h"

using namespace std;

int N=13;	//Queen number


//static tick_count t0;
int  sum=0;	//Solution number
int abs(int x)
{
  return x<0?-x:x;
}
bool place(int* x,int k)
{
  for(int i=1; i<k; i++)
    if(abs(k-i)==abs(x[k]-x[i]) || x[k] == x[i])
      return false;
  return true;
}
void queen_serial(int* x,int t)
{
  if(t>N && N>0)
  {

    sum++;

  }
  else
    for(int i=1; i<=N; i++)
    {
      x[t] = i;
      if(place(x,t))
        queen_serial(x,t+1);
    }

}
extern int queen_para(const int *, int);


int main(int argc, char *argv[])
{
  int p[20];

  RRecord rr("time.json", "nqueen");
  ParamParser pp;
  pp.parse(argc, argv);
  int thrd_num = pp.get<int>("thrd-num");
  rr.put("thrd-num", thrd_num);
  bool bIsPara = (thrd_num > 0);
  if(thrd_num > 0)
    initial_para_env(thrd_num);

  ofstream out_time_file;

  for(int k=0; k<20; k++)p[k]=0;


  start_record_cache_access();
  if(!bIsPara)
  {
    rr.run("elapsed-time", queen_serial, p, 1);
  }
  else
  {
    rr.run("elapsed-time", queen_para, p, 1);
  }

  end_record_cache_access(rr);
  return 0;
}


