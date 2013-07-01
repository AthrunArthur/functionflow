#include <iostream>
#include <vector>
#include "ff.h"
#define MIN_SORT_LEN 5
#define BUFFER_LEN 20
using namespace ff;

/* partition -- Partition the data from start to end to two parts 
 * with the left part is less than the pivot, and the right part is larger.
 * the return value is the index of the partition pivot */
int partition(int *data,int start,int end)
{
  int pivot;
  int i, j;
  int tmp;
  pivot = data[end];
  i = start-1;
  for(j=start;j<end;j++)
    if(data[j]<=pivot)
    {
      i++;/* i is the number of data less than the pivot before the index j. */
      tmp=data[i];
      data[i]=data[j];
      data[j]=tmp;
     }
  tmp=data[i+1];
  data[i+1]=data[end];
  data[end]=tmp;
  /* data[i+1]=pivot */
  return i+1;
}

/* quick_sort -- Use the serial quick sort algorithm to sort data. */ 

void quick_sort(int *data,int start,int end)
{
  int r;
  int i;
  if(start < end)
  {
      r = partition(data,start,end);
      quick_sort(data,start,r-1);
      quick_sort(data,r+1,end);
  }  
}

/* parallel quick sort using ff::para. */
void para_quick_sort(int * data,int i,int j)
{
  int r;
  if(j-i <= MIN_SORT_LEN-1)
    quick_sort(data,i,j);
  else
  {
    r = partition(data,i,j);
    ff::para<> a,b;
    a([&data,&i,&r](){para_quick_sort(data,i,r-1);});
    b([&data,&r,&j](){para_quick_sort(data,r+1,j);});
  }
}

int main(int argc, char *argv[])
{	
  int data[BUFFER_LEN]={9,2,34,2,8,17,3,5,1,10,43,4,30,12,54,15,45,30,28,20};
  for(int i=0;i<BUFFER_LEN;i++)
    std::cout << data[i] << "\t";
  std::cout << std::endl;
  para_quick_sort(data,0,BUFFER_LEN-1);
  for(int i=0;i<BUFFER_LEN;i++)
    std::cout << data[i] << "\t";
  std::cout << std::endl;
  return 0;
}
