#include<iostream>
#include<fstream>
#include<string>
#include<cstdlib>
#include<chrono>
#include"HashTable.h"
#include<stdio.h>
#include <tbb/task_scheduler_init.h>
#include <tbb/concurrent_vector.h>
#include <tbb/parallel_for_each.h>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#define FAILED 0
#define SUCCESS 1
#define NUM_CORES 8
#define MAXN 10000000
using namespace std;
using namespace tbb;
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

void rehash_list(HashTable* H, List* bucket, List** new_buckets, int new_n)
{
    for(int i = 0 ; i < bucket->mNumValues; ++i)
    {
        int idx = H->Hashcode(bucket->mList[i], new_n);
        int res = new_buckets[idx]->Insert(bucket->mList[i]);
        //	if(res > H->mMaxTop)
        //	   H->mMaxTop = res;
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
        //	cout << "new_N" << new_n << endl;
        new_buckets =  (List **) new List*[new_n];
        for(i = 0; i < new_n; ++i)
        {
            new_buckets[i] = new List();
        }
        concurrent_vector<int> vec_i;
        for(int i = 0; i < H->mNumBuckets; ++i)
        {

            vec_i.push_back(i);
        }
        H->mMaxTop = 0;
        parallel_for_each(vec_i.begin(),vec_i.end(),[H](int i) {
            rehash_list(H, H->mBuckets[i], new_buckets, new_n);
        });
        //printf("over1\n");
        H->Clear();
        H->mBuckets = new_buckets;
        H->mNumBuckets = new_n;
        //printf("over2\n");
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
    ifstream ins("../ff/datas/numbers.txt");
    if(!ins.is_open()) {
        cout << "Can't open the file numbers.txt" << endl;
        return -1;
    }
    for(int i = 0; i < MAXN; ++i)
        ins >> numbers[i];
    //cout <<"num0"<< numbers[0] << endl;
//	cout <<"num1"<< numbers[1] << endl;
}

int main()
{
    if(init()==-1)
        return -1;
    task_scheduler_init init(8);
    boost::property_tree::ptree pt;
    pt.put("time-unit", "us");
//	cout << "init over" << endl;
    HashTable * h = new HashTable(10, 40);

    string time_file_name="para_time.txt";
    ofstream out_time_file;
    chrono::time_point<chrono::system_clock> start, end;
    start = chrono::system_clock::now();
    concurrent_vector<int> vec_i;
    for(int i = 0; i < 8; ++i)
    {
        vec_i.push_back(i);
    }
    parallel_for_each(vec_i.begin(),vec_i.end(),[h](int i) {
        random_inserts_serial(h,i);
    });
    end = chrono::system_clock::now();
    int elapsed_seconds = chrono::duration_cast<chrono::microseconds>
                          (end-start).count();
    pt.put("para-elapsed-time", elapsed_seconds);
    boost::property_tree::write_json("time.json", pt);
    cout << "Elapsed time :" << elapsed_seconds << "us" << endl;

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
    out_time_file.open("para_time.txt", ios::app);
    if(!out_time_file.is_open()) {
        cout << "Can't open the file time.txt" << endl;
        return -1;
    }
    out_time_file<<elapsed_seconds <<endl;
    out_time_file.close();
    return 0;
}
