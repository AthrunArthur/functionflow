#include <chrono>
#include <cstdio>
#include <stdio.h>
#include <sstream>
#include <cstdlib>
#include <cassert>
#include <fstream>
#include <iostream>
#include <utility>
#ifdef CACHE_EVAL
#include <papi.h>
#endif
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

  RRecord rr("time.json", "kmeans");
  ParamParser pp;
  pp.parse(argc, argv);
  int thrd_num = pp.get<int>("thrd-num");
  rr.put("thrd-num", thrd_num);
  bool bIsPara = (thrd_num > 0);
  if(thrd_num > 0)
    initial_para_env(thrd_num);

  ofstream out_time_file;

  for(int k=0; k<20; k++)p[k]=0;

#ifdef CACHE_EVAL
  /*Add papi to trace cache miss*/
  int EventSet,retVal;
  long long startRecords[2], endRecords[2];
  retVal = PAPI_library_init(PAPI_VER_CURRENT);
  assert(retVal == PAPI_VER_CURRENT);
  EventSet = PAPI_NULL;
  retVal = PAPI_create_eventset(&EventSet);
  assert(retVal == PAPI_OK);
  //L1 TCM & TCA
  retVal = PAPI_add_event(EventSet, PAPI_L1_TCM);
  assert(retVal == PAPI_OK);
  retVal = PAPI_add_event(EventSet, PAPI_L1_TCA);
  assert(retVal == PAPI_OK);


  retVal = PAPI_start(EventSet);
  assert(retVal == PAPI_OK);
  retVal = PAPI_read(EventSet, startRecords);
  assert(retVal == PAPI_OK);
  /*Add papi to trace cache miss*/
#endif

  if(!bIsPara)
  {
    rr.run("elapsed-time", queen_serial, p, 1);
  }
  else
  {
    rr.run("elapsed-time", queen_para, p, 1);
  }

#ifdef CACHE_EVAL
  /*Stop papi trace*/
  retVal = PAPI_stop(EventSet, endRecords);
  assert(retVal == PAPI_OK);
  retVal = PAPI_cleanup_eventset(EventSet);
  assert(retVal == PAPI_OK);
  retVal = PAPI_destroy_eventset(&EventSet);
  assert(retVal == PAPI_OK);
  PAPI_shutdown(); 
  //L1 result
  std::cout << "L1 total cache miss = " << endRecords[0] - startRecords[0] << std::endl;
  std::cout << "L1 total cache access = " << endRecords[1] - startRecords[1] << std::endl;
#endif
  return 0;
}


