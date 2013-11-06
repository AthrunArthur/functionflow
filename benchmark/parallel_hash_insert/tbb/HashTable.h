#ifndef HASHTABLE_H
#define HASHTABLE_H
#include <omp.h>
#include <pthread.h>
#include <iostream>
#include <vector>
using namespace std;
class List{
public:
	int mNumValues;
	vector<int> mList;
	pthread_mutex_t mLock;
	pthread_spinlock_t sLock;

public:
	List();
	~List();
	void Clear();
	int	 Insert(int value); // return total_nums
};

class HashTable{
public:
	int mNumBuckets;
	List** mBuckets;
	int mTopBound;
	volatile int mMaxTop;
	
public:
	pthread_rwlock_t mResizeLock;
	
public:
	HashTable(int num_buckets, int top_bound );
	~HashTable();
	void Resize(int num_buckets);
	void Clear();
	void Insert(int value);
	int  Hashcode(int value, int n);
	void SetTopBound(int top_bound);
};
#endif
