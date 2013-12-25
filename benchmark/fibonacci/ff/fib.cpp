/***********************************************
The MIT License (MIT)

Copyright (c) 2012 Athrun Arthur <athrunarthur@gmail.com>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*************************************************/
#include "ff.h"
#include "common/log.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <chrono>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>


#define FIBNUM 40
#define MINPARA 25//3
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
    _DEBUG(LOG_INFO(main)<<"fib "<<n;)
    ff::para<int64_t> a, b;
    a([&n]()->int64_t {return fib(n - 1);});
    b([&n]()->int64_t {return fib(n - 2);});
    return (a && b).then([](int64_t x, int64_t y) {
        return x + y;
    });
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
    ff::rt::set_hardware_concurrency(8);//Set concurrency
    _DEBUG(ff::fflog<>::init(ff::INFO, "log.txt"))
    _DEBUG(LOG_INFO(main)<<"main start, id:"<<ff::rt::get_thrd_id());
    boost::property_tree::ptree pt;
    pt.put("time-unit", "us");

    bool bIsPara = false;//false;
    int elapsed_seconds;
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
    if(bIsPara)
        fib_res = fib(num);
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
