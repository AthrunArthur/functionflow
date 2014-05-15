#include "ff.h"
#include <cmath>
#include <chrono>
#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

using namespace std;

using namespace ff;

typedef ff::mutex TMutex;
typedef std::shared_ptr<TMutex> TMutex_ptr;
typedef std::shared_ptr<int64_t> Res_ptr;
std::vector<TMutex_ptr> ms;
std::vector<Res_ptr> rs;

typedef std::mutex SMutex;
typedef std::shared_ptr<SMutex> SMutex_ptr;
std::vector<SMutex_ptr> std_ms;


//const int LOOP_TIMES = 1000;
const int LOOP_TIMES = 30;


int fib(int n)
{
    if(n <=2)
        return 1;
    return fib(n-1) + fib(n-2);
}

int random_fib()
{
    int i = rand()%25 + 10;
    return fib(i);
}

void task_fun(int j) {
    for(int i = 0; i < LOOP_TIMES; ++i)
    {
//         random_fib();
        fib(10-j);
        ms[j]->lock();
//         *(rs[j]) += random_fib();
        *(rs[j]) += fib(15+j);
        ms[j]->unlock();
    }
}

void task_fun_serial(int j) {
    for(int i = 0; i < LOOP_TIMES; ++i)
    {
//         random_fib();

        fib(10-j);
//         *(rs[j]) += random_fib();
        *(rs[j]) += fib(15+j);
    }
}

void task_fun_std(int j) {
    for(int i = 0; i < LOOP_TIMES; ++i)
    {
        fib(10-j);
        std_ms[j]->lock();
        *(rs[j]) += fib(15+j);
        std_ms[j]->unlock();
    }
}

bool write_time_file(int elapsed_seconds, bool bIsPara) {
    ofstream out_time_file;
    if(bIsPara) {
        // write para time file
        out_time_file.open("para_time.txt",ios::app);
        if(!out_time_file.is_open()) {
            cout << "Can't open the file para_time.txt" << endl;
            return false;
        }
        out_time_file << elapsed_seconds << endl;
        out_time_file.close();
    }
    else {
        // write seq time file
        out_time_file.open("time.txt");
        if(!out_time_file.is_open()) {
            cout << "Can't open the file time.txt" << endl;
            return false;
        }
        out_time_file << elapsed_seconds << endl;
        out_time_file.close();
    }
    return true;
}

int main(int argc, char *argv[])
{
    //ff::rt::set_hardware_concurrency(8);//Set concurrency
    boost::property_tree::ptree pt;
    pt.put("time-unit", "us");
    
    bool bIsPara = false,bIsStd = false;//false;
    int elapsed_seconds;
    int concurrency = ff::rt::rt_concurrency();
    if(argc > 1) {
        stringstream ss_argv;
        int n;// n > 0 means parallel, otherwise serial.
        ss_argv << argv[1];
        ss_argv >> n;
        bIsPara = (n > 0)?true:false;
    }
    if(argc > 2)
        bIsStd = true;
    if(bIsPara) {
        //ff initialization
        para<> a;
        a([]() {});
        ff_wait(a);

        for(int i = 0; i< ff::rt::rt_concurrency(); i++)
        {
            if(bIsStd)
                std_ms.push_back(std::make_shared<SMutex>());
            else
                ms.push_back(std::make_shared<TMutex>());
            rs.push_back(std::make_shared<int64_t>(0));
        }
        std::chrono::time_point<chrono::system_clock> start, end;

        start = std::chrono::system_clock::now();
        paragroup p;
	int total = 100000;
	int per = total / ff::rt::rt_concurrency();
	for(int i = 0; i < ff::rt::rt_concurrency(); i++)
	{
	  for(int j = 0; j < per; j++)
	  {
	    para<> ptf;
	    if(bIsStd){
	      ptf([i](){
		task_fun_std(i);
	      });
	    }else {
	      ptf([i](){
		task_fun(i);}/*, ms[i]->id()*/
	      );
	    }
	    p.add(ptf);
	  }
	}
	std::cout<<"start waiting..."<<std::endl;
        ff_wait(all(p));
        end = std::chrono::system_clock::now();
        elapsed_seconds = std::chrono::duration_cast<chrono::microseconds>
                          (end-start).count();
	if(bIsStd)
	  pt.put("Std:para-elapsed-time", elapsed_seconds);
	else
	  pt.put("para-elapsed-time", elapsed_seconds);
        cout << "ff elapsed time: " << elapsed_seconds << "us" << endl;
    }
    else {
        for(int i = 0; i< concurrency; i++)
        {
            rs.push_back(std::make_shared<int64_t>(0));
        }
        std::chrono::time_point<chrono::system_clock> start, end;
        start = std::chrono::system_clock::now();

        for(int i=0; i < 60 * concurrency; i++)
        {
            for(int j = 0; j < concurrency; j++)
            {
                task_fun_serial(j);
            }
        }

        end = std::chrono::system_clock::now();
        elapsed_seconds = std::chrono::duration_cast<chrono::microseconds>
                          (end-start).count();
	pt.put("sequential-elapsed-time", elapsed_seconds);

        cout << "sequential elapsed time: " << elapsed_seconds << "us" << endl;

    }
    boost::property_tree::write_json("time.json", pt);
    write_time_file(elapsed_seconds,bIsPara);
    return 0;
}
