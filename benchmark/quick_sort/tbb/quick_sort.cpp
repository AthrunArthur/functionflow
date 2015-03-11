#include "../header.h"
#include <tbb/parallel_invoke.h>
#include "tbb/task_scheduler_init.h"

using namespace tbb;

void para_quick_sort(int * data,int i,int j,int para_len)
{

    int r;
    if(j-i <= para_len)/*The length is too small.*/
        quick_sort(data,i,j);
    else
    {
        r = partition(data,i,j);
        tbb::parallel_invoke(
        [&data,&i,&r,&para_len]() {
            para_quick_sort(data,i,r-1,para_len);
        },
        [&data,&r,&j,&para_len]() {
            para_quick_sort(data,r+1,j,para_len);
        }
        );
    }
}
