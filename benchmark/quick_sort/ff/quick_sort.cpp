#include "ff.h"
#include "../header.h"
using namespace ff;
#ifdef COUNT_TIME
#include "utilities/timer.h"
#endif

/* parallel quick sort using ff::para. */
void para_quick_sort(int * data,int i,int j,int para_len)
{
    if(j-i <= para_len)/*The length is too small.*/
        quick_sort(data,i,j);
    else
    {
        int r = partition(data,i,j);
        ff::para<> a,b;
        a([&data,&i,&r,&para_len]() {
            para_quick_sort(data,i,r-1,para_len);
        });
        b([&data,&r,&j,&para_len]() {
            para_quick_sort(data,r+1,j,para_len);
        });
	ff_wait(a&&b);
    }

}
