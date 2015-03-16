#include "utils.h"
#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include <algorithm>

using namespace std;

#define BUFFER_LEN 1<<4
#define INCREMENT 1<<8;

#define SEPARATOR ','

void generate_data(int N)
{
  std::string fp = "numbers.txt";
  fstream mf;
  mf.open(fp.c_str(), ios::in);
  if(mf.is_open())
  {
    return ;
  }
	ofstream out("numbers.txt");
	for(int i = 0; i < N; ++i)
	{
		out << rand() % N << ",";
	}
}

void comparator(int *data, int s, int e, bool dir)
{
  if((s>e) == dir)
  {
    std::swap(data[s], data[e]);
  }
}
void serial_bitonic_sort(int *data, int start, int end, bool up)
{
  int len = end-start;
  int max_k = static_cast<int>(log2(len));
  std::cout<<"initial : "<<std::endl;
  for(int i = start; i < end; ++i)
      std::cout<<data[i]<<",";
    std::cout<<std::endl;
  for(int k = 0; k < max_k; k++)
  {
    std::cout<<"*****k:"<<k<<std::endl;
    int mask = 1<<k;
    int ns = len / (2*mask);
    for(int n = 0; n < ns; ++n)
    {
      for(int i = 0; i < mask; ++i)
      {
        int pos = n*mask + i;
        comparator(data, pos, pos + mask, n%2 == up);
      }
    }
    for(int i = start; i < end; ++i)
      std::cout<<data[i]<<",";
    std::cout<<std::endl;
  }
}
extern void bitonic_sort(int *data, int start, int end, bool up);


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

  string in_file_name = "numbers.txt";
  string out_file_name = "numbers_sort.txt";
  ifstream in_file;
  ofstream out_file,out_time_file;
  int * data = (int *)malloc(sizeof(int)*BUFFER_LEN),buf_size = BUFFER_LEN;


    in_file.open(in_file_name.c_str());
    out_file.open(out_file_name.c_str());
  if(!in_file.is_open()) {
    cout << "Can't open the file " << in_file_name << endl;
    cout<<" generating numbers ...";
    generate_data(buf_size);
    in_file.open(in_file_name.c_str());
    if(!in_file.is_open()){
      cout<<"still cannot open the file!"<<endl;
      return -1;}
  }
  if(!out_file.is_open()) {
    cout << "Can't open the file " << out_file_name << endl;
    return -1;
  }
  for(int i=0; !in_file.eof() && i < buf_size; i++) {
    char tmp;
    if(i > buf_size){
      buf_size += INCREMENT;
      data = (int *)realloc(data,buf_size*sizeof(int));
    }        
    in_file >> data[i];
    in_file.get(tmp);
    if(tmp != SEPARATOR) {
      i++;
      break;
    }
  }
  start_record_cache_access();
  if(!bIsPara){
    rr.run("elapsed-time", serial_bitonic_sort, data, 0, buf_size, true);
  }else{
    rr.run("elapsed-time", bitonic_sort, data, 0, buf_size, true);
  }
  end_record_cache_access(rr);
  for(int i = 0; i < buf_size; ++i)
  {
    std::cout<<"buf_size :"<<buf_size<<" i:"<<i<<":"<<data[i]<<std::endl;
    out_file<<data[i]<<",";
  }
  out_file.close();
  return 0;
}

