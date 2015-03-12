#include "header.h"

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
    for(j=start; j<end; j++)
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

int main(int argc, char *argv[])
{
  RRecord rr("time.json", "qsort");
  ParamParser pp;
  pp.add_option("div", "the granularity of parallelism");
  pp.parse(argc, argv);
  int thrd_num = pp.get<int>("thrd-num");
  rr.put("thrd-num", thrd_num);
  bool bIsPara = (thrd_num > 0);
  if(thrd_num > 0)
    initial_para_env(thrd_num);
  int n_div = 1;
  if(pp.is_set("div"))
    n_div = pp.get<int>("div");

    string in_file_name = "numbers.txt";
    string out_file_name = "numbers_sort.txt";
    int len,i;
    ifstream in_file;
    ofstream out_file,out_time_file;
    int para_len;
    int * data = (int *)malloc(sizeof(int)*BUFFER_LEN),buf_size = BUFFER_LEN;
    

    if(in_file_name.empty() || out_file_name.empty() ) {
        cout << "File name is empty!" << endl;
        return -1;
    }
    else {
        in_file.open(in_file_name.c_str());
        out_file.open(out_file_name.c_str());
    }
    if(!in_file.is_open()) {
        cout << "Can't open the file " << in_file_name << endl;
        return -1;
    }
    if(!out_file.is_open()) {
        cout << "Can't open the file " << out_file_name << endl;
        return -1;
    }
    for(i=0; !in_file.eof(); i++) {
      char tmp;
      if(i >= buf_size){
	buf_size += INCREMENT;
	data = (int *)realloc(data,buf_size*sizeof(int));
// 	cout << "New size = " << buf_size << endl;
      }        
        in_file >> data[i];
        in_file.get(tmp);
        if(tmp != SEPARATOR) {
            i++;
            break;
        }
    }
    if(!in_file.eof() && i > buf_size) {
        cout << "Max data length is " << buf_size << endl;
        cout << "Only the first " << buf_size << " numbers will be sorted" << endl;
    }
    len = i;
    para_len = len / n_div;
    if(n_div == 1)
      cout << "len = " << len << endl;
    else
      cout << "Para granularity = " << n_div << endl;
    in_file.close();

    //Pre initialization thread_pool when using parallelizing.

    
  start_record_cache_access();
    if(!bIsPara){
      rr.run("elapsed-seconds", quick_sort, data, 0, len);
    }
    else
    {
      rr.run("elapsed-seconds", para_quick_sort, data, 0, len-1, para_len);
    }
    

  end_record_cache_access(rr);

    return 0;//return the parallel time to the system
}
