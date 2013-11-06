#include "HashTable.h"
List::List()
{
	Clear();
	pthread_mutex_init(&mLock, 0);
	pthread_spin_init(&sLock, 0);
};

List::~List()
{
	Clear();
}

int 
List::Insert(int value)
{
//	pthread_mutex_lock(&mLock);
	pthread_spin_lock(&sLock);
	++mNumValues;
	mList.push_back(value);
//	cout << "insert"<< value <<endl;
//	cout<< mList[0] << endl;
//	pthread_mutex_unlock(&mLock);
	pthread_spin_unlock(&sLock);
	return mNumValues;
}

void
List::Clear()
{
	mList.clear();
	mNumValues = 0;
	pthread_spin_destroy(&sLock);
	pthread_mutex_destroy(&mLock);
}
/*end of List
 *
 *  
 *start of HashTable
 */
HashTable::HashTable(int num_buckets = 10, int top_bound = 20)
{
	pthread_rwlock_init(&mResizeLock, 0);
	Resize(num_buckets);
	mTopBound = top_bound;
	mMaxTop = 0;
}

HashTable::~HashTable()
{
	Clear();
}

void
HashTable::Resize(int num_buckets)
{
	Clear();
    this->mNumBuckets = num_buckets;
	mBuckets =(List**) new List*[mNumBuckets];
	for(int i = 0; i < mNumBuckets; ++i)
	{
		mBuckets[i] = new List();
	}
}

void
HashTable::Clear()
{
	for(int i = 0; i < mNumBuckets; ++i)
	{
		delete mBuckets[i];
	}
	delete mBuckets;
}

void
HashTable::Insert(int value)
{
	int idx = Hashcode(value, mNumBuckets);
	//cout << value << endl;
//	cout << idx << endl;
	//cout << "idx = " << idx << endl;
	int ret = mBuckets[idx]->Insert(value);
	if (ret > mMaxTop)
	{
		mMaxTop = ret;
	}
}

int 
HashTable::Hashcode(int value, int n)
{
	return value % n;
}

void 
HashTable::SetTopBound(int top_bound)
{
	this->mTopBound = top_bound;
}
