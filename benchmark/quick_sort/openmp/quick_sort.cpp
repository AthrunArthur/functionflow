#include "../header.h"
#include <omp.h>

void para_quick_sort(int * data,int i,int j,int para_len)
{

    int r;
    if(j-i <= para_len)/*The length is too small.*/
        quick_sort(data,i,j);
    else
    {
        r = partition(data,i,j);
        #pragma omp task
        {
            para_quick_sort(data,i,r-1,para_len);
        }

        #pragma omp task
        {
            para_quick_sort(data,r+1,j,para_len);
        }
    }
}
