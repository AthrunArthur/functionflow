#include "ff.h"
#include "../header.h"
using namespace ff;


/* parallel quick sort using ff::para. */
void para_quick_sort(int * data,int i,int j,int para_len)
{
    int r;
    if(j-i <= para_len)/*The length is too small.*/
        quick_sort(data,i,j);
    else
    {
        r = partition(data,i,j);
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
