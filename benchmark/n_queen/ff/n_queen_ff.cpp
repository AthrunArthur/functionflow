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
#include "ff.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>


using namespace std;
using namespace ff;

int N=13;	//Queen number


//static tick_count t0;
int  sum=0;	//Solution number
accumulator<int> psum(0, [](const int & x, const int& y) {
    return x + y;
});
inline int abs(int x)
{
    return x<0?-x:x;
}
bool place(int* x,int k)
{
    for(int i=1; i<k; i++)
        if(abs(k-i)==abs(x[k]-x[i]) || x[k] == x[i])
            return false;
    return true;
}
void queen_parallel(int* x,int t)
{
    if(t>N && N>0)
    {
        psum.increase(1);
    }
    else
        for(int i=1; i<=N; i++)
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
        for(int i=1; i<=N; i++)
        {
            x[t] = i;
            if(place(x,t))
                queen_serial(x,t+1);
        }

}

int queen_para(const int* x,int t) {

    int xx[20];
    for(int k=0; k<20; k++)xx[k]=x[k];
    //if(t>N&&N>0){
    //sum++;
    //}
    if(t>(N-8>0?N-8:N)&&N>0) {
        queen_parallel(xx,t);
    }
    else {
        ff::paragroup pp;
        //tbb::task_list list;
        for(int i=1; i<=N; i++)
        {

            xx[t]=i;
            if(place(xx,t))
            {
                ff::para<void> p;
                p([xx,t]() {
                    queen_para(xx,t+1);
                });
                //list.push_back( *new( allocate_child() ) QueenTask(x,t+1) );
                pp.add(p);
                //count=count+1;

            }
        }
        //set_ref_count(count);
        ff_wait(all(pp));
        //spawn_and_wait_for_all(list);
    }
    return 0;
}


int main(int argc, char *argv[])
{
    ff::rt::set_hardware_concurrency(8);//Set concurrency
    boost::property_tree::ptree pt;
    pt.put("time-unit", "us");
    int p[20];
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

    for(int k=0; k<20; k++)p[k]=0;
    start = chrono::system_clock::now();
    if(n_div==0) {
        queen_serial(p,1);
    }
    else {
        _DEBUG(ff::fflog<>::init(ff::ERROR, "log.txt"))
        _DEBUG(LOG_INFO(main)<<"main start, id:"<<ff::rt::get_thrd_id());
        //QueenTask& a = *new(task::allocate_root()) QueenTask(p,1);
        //task::spawn_root_and_wait(a);
        queen_para(p,1);
        //psum.get();
    }
    end = chrono::system_clock::now();
    if(n_div == 0)
        std::cout<<"Result:"<<sum<<std::endl;
    else
        std::cout<<"Result:"<<psum.get()<<std::endl;
    int elapsed_seconds = chrono::duration_cast<chrono::microseconds>
                          (end-start).count();
    if(n_div)
        pt.put("para-elapsed-time", elapsed_seconds);
    else
        pt.put("sequential-elapsed-time", elapsed_seconds);
    boost::property_tree::write_json("time.json", pt);
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


