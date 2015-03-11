#include "tbb/task.h"
#include "tbb/task_scheduler_init.h"

#define MINPARA 25//3
using namespace std;
using namespace tbb;

int64_t SerialFib( int64_t n ) {
    if( n<=2 )
        return 1;
    else
        return SerialFib(n-1)+SerialFib(n-2);
}

class FibTask: public task {
public:
    const int64_t n;
    int64_t* const sum;
    FibTask( int64_t n_, int64_t* sum_ ) :
        n(n_), sum(sum_)
    {}
    task* execute() {      // Overrides virtual function task::execute
        if( n<MINPARA ) {
            *sum = SerialFib(n);
        } else {
            int64_t x, y;
            FibTask& a = *new( allocate_child() ) FibTask(n-1,&x);
            FibTask& b = *new( allocate_child() ) FibTask(n-2,&y);
            // Set ref_count to 'two children plus one for the wait".
            set_ref_count(3);
            // Start b running.
            spawn( b );
            // Start a running and wait for all children (a and b).
            spawn_and_wait_for_all(a);
            // Do the sum
            *sum = x+y;
        }
        return NULL;
    }
};


int64_t fib( int64_t n ) {
    int64_t sum;
    FibTask& a = *new(task::allocate_root()) FibTask(n,&sum);
    task::spawn_root_and_wait(a);
    return sum;
}
