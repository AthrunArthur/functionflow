#include "tbb/task.h"
#include "tbb/tbb.h"
#include "tbb/task_scheduler_init.h"
#include "tbb/tick_count.h"
#include "tbb/mutex.h"
#include "tbb/spin_mutex.h"
#include "tbb/tbb_thread.h"

extern int sum;
extern int N;
extern bool place(int * x, int k);

using namespace tbb;

typedef spin_mutex FreeListMutexType;
FreeListMutexType FreeListMutex;

int p[20];

static tick_count t0;
void queen_parallel(int* x,int t)
{
  if(t>N && N>0)
  {
    FreeListMutexType::scoped_lock lock;
    lock.acquire(FreeListMutex);
    sum++;
    lock.release();
  }
  else
    for(int i=1; i<=N; i++)
    {
      x[t] = i; //The t-th queen is in colomn i.
      if(place(x,t))
        queen_parallel(x,t+1);
    }

}

class QueenTask: public task {
  public:
    int x[20];	//Total number of queen colomns
    int t;
    QueenTask( const int* x_, int t_ )
    {
      t=t_;
      for(int i1=0; i1<20; i1++)x[i1]=x_[i1];
    }
    QueenTask* execute() {      // Overrides virtual function task::execute
      if(t>(N-8>0?N-8:N)&&N>0) {
        queen_parallel(x,t);
      }
      else {
        int count=1;
        tbb::task_list list;
        for(int i=1; i<=N; i++)
        {
          x[t]=i;
          if(place(x,t))
          {
            list.push_back( *new( allocate_child() ) QueenTask(x,t+1) );
            count=count+1;

          }
        }
        set_ref_count(count);
        spawn_and_wait_for_all(list);
      }

      return NULL;
    }
};

int queen_para(const int * x, int t)
{
  QueenTask& a = *new(task::allocate_root()) QueenTask(x,t);
  task::spawn_root_and_wait(a);
  return 0;
}
