#include<iostream>
#include<fstream>
#include<string>
#include<cstdlib>
#include<chrono>
#include<omp.h>
#include"HashTable.h"
#include<stdio.h>
#define FAILED 0
#define SUCCESS 1
using namespace std;

// added here

pthread_cond_t init_cond, done_cond;
pthread_mutex_t init_mutex, done_mutex;
List** new_buckets;
int    new_n;


void rehash_list(HashTable* H, List* bucket, List** new_buckets, int new_n)
{
	for(int i = 0 ; i < bucket->mNumValues; ++i)
	{
		int idx = H->Hashcode(bucket->mList[i], new_n);
		new_buckets[idx]->Insert(bucket->mList[i]);
	}
}

bool is_overflow(HashTable *H)
{
	if(H->mMaxTop > H->mTopBound)
		return true;
	return false;
}

void resize_table_if_overflow(HashTable * H)
{
	if (is_overflow(H))
	{
	//	if(!(helperLock(0, &H->mResizeLock)))
	//			return;
		pthread_rwlock_wrlock(&H->mResizeLock);
		if(!is_overflow(H))
		{
			pthread_rwlock_unlock(&H->mResizeLock);
			return ;
		}
		new_n = H->mNumBuckets * 2;
		printf("thread %d new_n = %d \n", omp_get_thread_num(), new_n);
		new_buckets =  (List **) new List*[new_n];
		for(int i = 0; i < new_n; ++i)
		{
			new_buckets[i] = new List();
		}
		//helperInit(0);
		for(int i = 0; i < H->mNumBuckets; ++i)
		{
		//	helperPush(0 , [=](){rehash_list(H,H->mBuckets[i], new_buckets, new_n);});
			#pragma omp task
			rehash_list(H, H->mBuckets[i], new_buckets, new_n);
		}
		#pragma omp taskwait
		//helperWork(0);
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
	success = pthread_rwlock_tryrdlock(&H->mResizeLock);
	if (success != 0) 
	{
		return FAILED;
	}
	H->Insert(k);
	pthread_rwlock_unlock(&H->mResizeLock);
	return SUCCESS;
} 

void random_inserts_serial(HashTable * H , int n)
{
	for(int i = 0; i < n; ++i)
	{
		int res;
		int k = abs((int)rand());
		do{
			res = try_insert(H, k);
		}while(res == FAILED);
		resize_table_if_overflow(H);
	}
}

void init()
{
	pthread_mutex_init(&init_mutex, 0 );
	pthread_mutex_init(&done_mutex, 0 );
	pthread_cond_init(&init_cond, 0);
	pthread_cond_init(&done_cond, 0);
}

int main()
{
	init();
	omp_set_num_threads(8);
	HashTable * h = new HashTable(10,20);
	
	string time_file_name="para_time.txt";
	ofstream out_time_file;
	chrono::time_point<chrono::system_clock> start, end;
	start = chrono::system_clock::now();

	#pragma omp parallel 
	{
		#pragma omp single
		{
			for(int i = 0; i < 8; ++i)
			#pragma omp task
			{
				random_inserts_serial(h, 2000000);
			}
		}
	}
	#pragma omp taskwait
	
	end = chrono::system_clock::now();
	int elapsed_seconds = chrono::duration_cast<chrono::microseconds>
						  (end-start).count();
	cout << "Elapsed time :" << elapsed_seconds << "us" << endl;
	
	int total = 0;
	for (int i = 0; i <h->mNumBuckets; ++i)
	{
		for (int j = 0; j < h->mBuckets[i]->mNumValues;++j)
		{
		//	cout << h->mBuckets[i]->mList[j] << "s ";
		}

		total += h->mBuckets[i]->mNumValues;
	}
	out_time_file.open("para_time.txt", ios::app);
	out_time_file<<elapsed_seconds <<endl;
	out_time_file.close();
	return 0;
}
