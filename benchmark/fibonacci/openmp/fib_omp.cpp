#include <omp.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <chrono>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#define FIBNUM 40
#define MINPARA 25
using namespace std;

int64_t sfib(int64_t n )
{
    if(n <= 2)
        return 1;
    return sfib(n -1 ) + sfib(n-2);
}

int64_t fib(int64_t n)
{
    if(n < MINPARA)
        return sfib(n);
    int64_t x,y;
    #pragma omp parallel
    {
        #pragma omp task
        {
            x = fib(n - 1);
        }
        #pragma omp task
        {
            y = fib(n - 2);
        }
        #pragma omp taskwait
    }
    return x+y;
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

    bool bIsPara = false;//false;
    int elapsed_seconds;
    boost::property_tree::ptree pt;
    pt.put("time-unit", "us");

    if(argc > 1) {
        stringstream ss_argv;
        int n;// n > 0 means parallel, otherwise serial.
        ss_argv << argv[1];
        ss_argv >> n;
        bIsPara = (n > 0)?true:false;
    }
    int64_t num = FIBNUM,fib_res;
    std::chrono::time_point<chrono::system_clock> start, end;

    start = std::chrono::system_clock::now();
    if(bIsPara) {
        omp_set_num_threads(8);
        fib_res = fib(num);
    }
    else
        fib_res = sfib(num);
    end = std::chrono::system_clock::now();
    elapsed_seconds = std::chrono::duration_cast<chrono::microseconds>
                      (end-start).count();
    if(bIsPara)
        pt.put("para-elapsed-time", elapsed_seconds);
    else
        pt.put("sequential-elapsed-time", elapsed_seconds);
    boost::property_tree::write_json("time.json", pt);
    cout << "ff elapsed time: " << elapsed_seconds << "us" << endl;
//int64_t fib_res = sfib(num);
    cout<<"fib( "<<num<<" )="<<fib_res<<endl;
    write_time_file(elapsed_seconds,bIsPara);
    return 0;
}
