#include "tbb/task.h"
#include "tbb/task_scheduler_init.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <chrono>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#ifdef CACHE_EVAL
#include <papi.h>
#endif
#include <assert.h>

#define FIBNUM 40
#define MINPARA 25//3
using namespace std;
using namespace tbb;

int64_t SerialFib( int64_t n ) {
    if( n<=2 )
        return 1;
    else
        return SerialFib(n-1)+SerialFib(n-2);
}

class FibTask: public task {
public:
    const int64_t n;
    int64_t* const sum;
    FibTask( int64_t n_, int64_t* sum_ ) :
        n(n_), sum(sum_)
    {}
    task* execute() {      // Overrides virtual function task::execute
        if( n<MINPARA ) {
            *sum = SerialFib(n);
        } else {
            int64_t x, y;
            FibTask& a = *new( allocate_child() ) FibTask(n-1,&x);
            FibTask& b = *new( allocate_child() ) FibTask(n-2,&y);
            // Set ref_count to 'two children plus one for the wait".
            set_ref_count(3);
            // Start b running.
            spawn( b );
            // Start a running and wait for all children (a and b).
            spawn_and_wait_for_all(a);
            // Do the sum
            *sum = x+y;
        }
        return NULL;
    }
};


int64_t ParallelFib( int64_t n ) {
    int64_t sum;
    FibTask& a = *new(task::allocate_root()) FibTask(n,&sum);
    task::spawn_root_and_wait(a);
    return sum;
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
    task_scheduler_init init(8);
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

#   ifdef CACHE_EVAL
    /*Add papi to trace cache miss*/
    int EventSet,retVal;
    long long startRecords[2], endRecords[2];
    retVal = PAPI_library_init(PAPI_VER_CURRENT);
    assert(retVal == PAPI_VER_CURRENT);
    EventSet = PAPI_NULL;
    retVal = PAPI_create_eventset(&EventSet);
    assert(retVal == PAPI_OK);
    //L1 TCM & TCA
    retVal = PAPI_add_event(EventSet, PAPI_L1_TCM);
    assert(retVal == PAPI_OK);
    retVal = PAPI_add_event(EventSet, PAPI_L1_TCA);
    assert(retVal == PAPI_OK);
    
    //L2 TCM & TCA
//     retVal = PAPI_add_event(EventSet, PAPI_L2_TCM);
//     assert(retVal == PAPI_OK);
//     retVal = PAPI_add_event(EventSet, PAPI_L2_TCA);
//     assert(retVal == PAPI_OK);
    
    //L3 TCM & TCA
//     retVal = PAPI_add_event(EventSet, PAPI_L3_TCM);
//     assert(retVal == PAPI_OK);
//     retVal = PAPI_add_event(EventSet, PAPI_L3_TCA);
//     assert(retVal == PAPI_OK);    
    
    retVal = PAPI_start(EventSet);
    assert(retVal == PAPI_OK);
    retVal = PAPI_read(EventSet, startRecords);
    assert(retVal == PAPI_OK);
    /*Add papi to trace cache miss*/
#   endif    

    start = std::chrono::system_clock::now();
    if(bIsPara)
        fib_res = ParallelFib(num);
    else
        fib_res = SerialFib(num);
    end = std::chrono::system_clock::now();
    elapsed_seconds = std::chrono::duration_cast<chrono::microseconds>
                      (end-start).count();
    
#   ifdef CACHE_EVAL
    /*Stop papi trace*/
    retVal = PAPI_stop(EventSet, endRecords);
    assert(retVal == PAPI_OK);
    retVal = PAPI_cleanup_eventset(EventSet);
    assert(retVal == PAPI_OK);
    retVal = PAPI_destroy_eventset(&EventSet);
    assert(retVal == PAPI_OK);
    PAPI_shutdown(); 
    //L1 result
    std::cout << "L1 total cache miss = " << endRecords[0] - startRecords[0] << std::endl;
    std::cout << "L1 total cache access = " << endRecords[1] - startRecords[1] << std::endl;
    //L2 result
//     std::cout << "L2 total cache miss = " << endRecords[0] - startRecords[0] << std::endl;
//     std::cout << "L2 total cache access = " << endRecords[0] - startRecords[0] << std::endl;
    //L3 result
//     std::cout << "L3 total cache miss = " << endRecords[0] - startRecords[0] << std::endl;
//     std::cout << "L3 total cache access = " << endRecords[0] - startRecords[0] << std::endl;
    /*Stop papi trace*/
#   endif
    
    cout << "tbb elapsed time: " << elapsed_seconds << "us" << endl;
    if(bIsPara)
        pt.put("para-elapsed-time", elapsed_seconds);
    else
        pt.put("sequential-elapsed-time", elapsed_seconds);
    boost::property_tree::write_json("time.json", pt);
    //int64_t fib_res = sfib(num);
    cout<<"fib( "<<num<<" )="<<fib_res<<endl;
    write_time_file(elapsed_seconds,bIsPara);
    return 0;
}
