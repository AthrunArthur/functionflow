#include<iostream>
#include <sstream>
#include<fstream>
#include<string>
#include<cstdlib>
#include<chrono>
#include"HashTable.h"
#include<stdio.h>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#define FAILED 0
#define SUCCESS 1
#define NUM_CORES 8
#define MAXN 10000000
using namespace std;
// added here

pthread_cond_t init_cond, done_cond;
pthread_mutex_t init_mutex, done_mutex;
List** new_buckets;
int    new_n;
int numbers[MAXN];

void rehash_list(HashTable* H, int startIndex , int endIndex, List** new_buckets, int new_n)
{
    for(int k = startIndex; k <= endIndex; ++k)
    {
        List * bucket = H->mBuckets[k];
        for(int i = 0 ; i < bucket->mNumValues; ++i)
        {
            int idx = H->Hashcode(bucket->mList[i], new_n);
            new_buckets[idx]->Insert(bucket->mList[i]);
        }
    }
}

bool is_overflow(HashTable *H)
{
//	cout << "H->mMaddxTop" << H->mMaxTop << endl;
    if(H->mMaxTop > H->mTopBound)
        return true;
    return false;
}

void resize_table_if_overflow(HashTable * H)
{
    if (is_overflow(H))
    {
        //if(!(helperLock(0, &H->mResizeLock)))
        //			return;
        pthread_rwlock_wrlock(&H->mResizeLock);
        if(!is_overflow(H))
        {
            pthread_rwlock_unlock(&H->mResizeLock);
            return ;
        }
        new_n = H->mNumBuckets * 2;
        int i = 0;
        //cout << "new_N" << new_n << endl;
        new_buckets =  (List **) new List*[new_n];
        for(i = 0; i < new_n; ++i)
        {
            new_buckets[i] = new List();
        }
        int startIndex = 0;
        int endIndex = -1;
        int cnt = H->mNumBuckets / NUM_CORES;

        for(i = 0; i < NUM_CORES; ++i)
        {
            endIndex = (i == NUM_CORES - 1) ? H->mNumBuckets - 1 : startIndex + cnt - 1;
            #pragma omp task
            rehash_list(H,startIndex, endIndex, new_buckets, new_n);
            startIndex += cnt;
            //ff::para<void> _p;
            //_p([=](){rehash_list(H,H->mBuckets[i], new_buckets, new_n);});
            //_pp.add(_p);
        }
        //	helperWork(0);
        #pragma omp taskwait
        H->Clear();
        H->mBuckets = new_buckets;
        H->mNumBuckets = new_n;
        H->mMaxTop = 0;
        //printf("over");
        pthread_rwlock_unlock(&H->mResizeLock);
    }
}
int try_insert(HashTable * H , int k)
{
    int success = 0;
    if (success != 0)
    {
        return FAILED;
    }
    H->Insert(k);
    return SUCCESS;
}

void random_inserts_serial(HashTable * H , int n)
{
    for(int i = n; i < MAXN; i += NUM_CORES)
    {
        //cout << "i = " << i << endl;
        int k = numbers[i];
        int res = try_insert(H, k);
        resize_table_if_overflow(H);
    }
}

int init()
{
    pthread_mutex_init(&init_mutex, 0 );
    pthread_mutex_init(&done_mutex, 0 );
    pthread_cond_init(&init_cond, 0);
    pthread_cond_init(&done_cond, 0);
    ifstream ins("../parallel_hash_insert/ff/datas/numbers.txt");
    if(!ins.is_open()) {
        cout << "Can't open the file numbers.txt" << endl;
        return -1;
    }
    for(int i = 0; i < MAXN; ++i)
        ins >> numbers[i];
    //cout <<"num0"<< numbers[0] << endl;
    //cout <<"num1"<< numbers[1] << endl;
}

int main(int argc,char * argv[])
{
    if(init()==-1)
        return -1;
    omp_set_num_threads(8);
    boost::property_tree::ptree pt;
    pt.put("time-unit", "us");
    //cout << "init over" << endl;
    HashTable * h = new HashTable(10, 40);
    bool bIsPara = false;//false;
    if(argc > 1) {
        stringstream ss_argv;
        int n;// n > 0 means parallel, otherwise serial.
        ss_argv << argv[1];
        ss_argv >> n;
        bIsPara = (n > 0)?true:false;
    }

    ofstream out_time_file;
    chrono::time_point<chrono::system_clock> start, end;
    start = chrono::system_clock::now();
    if(bIsPara) {
        #pragma omp parallel
        {
            #pragma omp single
            {
                for(int i = 0; i < 8; ++i)
                {
                    #pragma omp task
                    random_inserts_serial(h,i);
                }
            }
        }
        #pragma omp taskwait
    }
    else {
        for(int i = 0; i < 8; ++i)
            random_inserts_serial(h,i);
    }
    end = chrono::system_clock::now();
    int elapsed_seconds = chrono::duration_cast<chrono::microseconds>
                          (end-start).count();
    cout << "Elapsed time:" << elapsed_seconds << "us" << endl;

    int total = 0;
    int j = 0;
    for (int i = 0; i <h->mNumBuckets; ++i)
    {
        for (j = 0; j < h->mBuckets[i]->mNumValues; ++j)
        {
            //	cout << h->mBuckets[i]->mList[j] << "s ";
        }

        total += h->mBuckets[i]->mNumValues;
    }
    if(bIsPara) {
        // write para time file
        out_time_file.open("para_time.txt",ios::app);
        if(!out_time_file.is_open()) {
            cout << "Can't open the file para_time.txt" << endl;
            return -1;
        }
        out_time_file << elapsed_seconds << endl;
        out_time_file.close();
    }
    else {
        // write seq time file
        out_time_file.open("time.txt");
        if(!out_time_file.is_open()) {
            cout << "Can't open the file time.txt" << endl;
            return -1;
        }
        out_time_file << elapsed_seconds << endl;
        out_time_file.close();
    }
    if(bIsPara)
        pt.put("para-elapsed-time", elapsed_seconds);
    else
        pt.put("sequential-elapsed-time", elapsed_seconds);
    return 0;
}
