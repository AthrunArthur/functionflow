#ifdef NDEBUG
#undef NDEBUG
#endif
#include <chrono>
#include <cstdio>
#include <stdio.h>
#include <sstream>
#include <cstdlib>
#include <cassert>
#include <fstream>
#include <iostream>
#include <utility>
#include "tbb/task.h"
#include "tbb/tbb.h"
#include "tbb/task_scheduler_init.h"
#include "tbb/tick_count.h"
#include "tbb/mutex.h"
#include "tbb/spin_mutex.h"
#include "tbb/tbb_thread.h"
 
using namespace tbb;
using namespace std;
typedef spin_mutex FreeListMutexType;
FreeListMutexType FreeListMutex;
int N=13;	//Queen number
int p[20];

static tick_count t0;
int  sum=0;	//Solution number
inline int abs(int x)
{
	return x<0?-x:x;
}
bool place(int* x,int k)
{
	for(int i=1;i<k;i++)
		if(abs(k-i)==abs(x[k]-x[i]) || x[k] == x[i])
			return false;
	return true;
}
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
      for(int i=1;i<=N;i++)   
      {   
          x[t] = i; //The t-th queen is in colomn i.
          if(place(x,t)) 
            queen_parallel(x,t+1);    
      }   
  
}  
void queen_serial(int* x,int t)   
{   
    if(t>N && N>0)  
    {

	sum++;

    }  
    else  
      for(int i=1;i<=N;i++)   
      {   
          x[t] = i; 
          if(place(x,t))   
            queen_serial(x,t+1);    
      }   
  
} 
class QueenTask: public task {
public:
    int x[20];	//Total number of queen colomns
    int t;
    QueenTask( int* x_, int t_ ) 
    {
	t=t_;
        for(int i1=0;i1<20;i1++)x[i1]=x_[i1];
    }
    QueenTask* execute() {      // Overrides virtual function task::execute
	if(t>(N-8>0?N-8:N)&&N>0){
		queen_parallel(x,t);
	}
	else{	
		int count=1;
		tbb::task_list list;
		for(int i=1;i<=N;i++)
		{
			x[t]=i;
			if(place(x,t))
			{
				list.push_back( *new( allocate_child() ) QueenTask(x,t+1) );
				//task[count]= new( allocate_child() ) QueenTask(x,t+1);
				count=count+1;

			}	
		}
            // Set ref_count to 'two children plus one for the wait".
            	set_ref_count(count);
            // Start b running.
	    //for(int j=count-2;j>=0;j--)
            //spawn(*task[j]);
            // Start a running and wait for all children (a and b).
            //spawn_and_wait_for_all(*task[0]);
		spawn_and_wait_for_all(list);
	}
                   
        return NULL;
    }
};

int main(int argc, char *argv[])
{
    task_scheduler_init init(8);
	chrono::time_point<chrono::system_clock> start, end;
	string time_file_name = "para_time.txt";
	//std::cout<<"Input a number:";
	//std::cin>>N;
	ofstream out_time_file;
	int n_div = 0;// Default 0
	if(argc > 1) {
        stringstream ss_argv;
        ss_argv << argv[1];
        ss_argv >> n_div;
    	}
	if( time_file_name.empty()) {
        cout << "File name is empty!" << endl;
        return -1;
    	}
    	
	for(int k=0;k<20;k++)p[k]=0;
	start = chrono::system_clock::now();
	if(n_div==0){
		queen_serial(p,1);
	}
	else{
	QueenTask& a = *new(task::allocate_root()) QueenTask(p,1);
    	task::spawn_root_and_wait(a);
	}
	end = chrono::system_clock::now();
	std::cout<<"Result:"<<sum<<std::endl;
	int elapsed_seconds = chrono::duration_cast<chrono::microseconds>
                          (end-start).count();
	cout << "Elapsed time: " << elapsed_seconds << "us" << endl;
	if(n_div != 0) {
        out_time_file.open(time_file_name.c_str(),ios::app);
        if(!out_time_file.is_open()) {
            cout << "Can't open the file " << time_file_name << endl;
            return -1;
        }
        out_time_file << elapsed_seconds << endl;
        out_time_file.close();
    	}
    	else {
        out_time_file.open("time.txt");
        if(!out_time_file.is_open()) {
            cout << "Can't open the file time.txt" << endl;
            return -1;
        }
        out_time_file << elapsed_seconds << endl;
        out_time_file.close();
    	}
    	return 0;
}


