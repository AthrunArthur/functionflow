#ifndef HELPER_HPP
#define HELPER_HPP
#include<tuple>
#include<functional>
#include<omp.h>
#include<memory>
#include<thread>
/*
 *
 *Use C11 template to solve the problem of compile 
 *
 */
bool helperNextTask(int id);
void helperInit(int id);
struct HTask{
public:
	std::function<void()> mf;
	HTask* next;

	HTask():next(NULL)
	{
	}

	void Run()
	{

		//printf("thread %d  run start\n", omp_get_thread_num());
		mf();
		//	printf("thread %d  run end\n", omp_get_thread_num());
	}

	~HTask()
	{
	}
};

struct ScopeLock{
	pthread_mutex_t mutex;
	public:
		ScopeLock()
		{	
			pthread_mutex_init(&mutex, 0);
			pthread_mutex_lock(&mutex);
		}
		~ScopeLock()
		{
			pthread_mutex_unlock(&mutex);
		} 
};

struct HelperLock{
	HTask *head;
	HTask *tail;
	int   total;
	int   cnt;   // task end cnt
	int   tcnt; //task start cnt
	//pthread_mutex_t queueLock;
	pthread_spinlock_t queueLock;
	pthread_cond_t barrier_cond;
	pthread_mutex_t barrier_mutex;
	pthread_mutex_t try_mutex;
	
public:
	HelperLock()
	{
		reset();
	}

	void reset()
	{
		pthread_cond_init(&barrier_cond,0);
		pthread_mutex_init(&barrier_mutex,0);
		pthread_mutex_init(&try_mutex,0);
		pthread_spin_init(&queueLock,0);
		cnt = 0;
		total = 0;
		tcnt = 0;
		head = NULL;
		tail = NULL;
	}
	
	bool totalIsZero()
	{
		return total == 0;
	}

	bool totalIsCnt()
	{
		return  total == cnt;
	}

	bool totalIsTCnt()
	{
		return total == tcnt;
	}

	void totalInc()
	{
		__sync_fetch_and_add(&total, 1);
	}
	
	void tcntInc()
	{
		__sync_fetch_and_add(&tcnt, 1);
	}

	void cntInc()
	{
		__sync_fetch_and_add(&cnt, 1);
	}

	void output()
	{
		printf("thrid = %d , cnt = %d, total = %d \n" , omp_get_thread_num(), cnt, total);
	}

	void lock()
	{
		//pthread_mutex_lock(&queueLock);
		pthread_spin_lock(&queueLock);
	}
	void lockTry()
	{
		//pthread_mutex_lock(&queueLock);
		pthread_mutex_lock(&try_mutex);
	}

	void unlock()
	{
		//pthread_mutex_unlock(&queueLock);
		pthread_spin_unlock(&queueLock);
	}

	void unlockTry()
	{
		pthread_mutex_unlock(&try_mutex);
	}

	HTask* First()
	{
		lock();  
	//	printf("thread %d, FIRST id =  \n", omp_get_thread_num() );
		HTask *now = head;
		if(head != NULL)
			head = head->next;
		unlock();
		return now;
	}

	void Push(HTask * t)
	{
		lock();
		if(head == NULL){
			head = t;
			tail = t;
		}else{
			tail->next = t;
			tail = tail->next;
		}
		unlock();
	}
};

HelperLock helpers[1000];
/*
 * lambda表达式
 */
void helperPush(int id ,  std::function<void()> tmp)
{

	HTask * t = new HTask();
	
	t->mf = tmp;
	//printf("total = %d" , helpers[id].total);
	helpers[id].Push(t);

	helpers[id].totalInc();
}

bool helperWork(int id)
{
//	printf("thread %d, helperwork\n", omp_get_thread_num());
//	helpers[id].output();
	while(helpers[id].totalIsZero() || !(helpers[id].totalIsCnt()))
	{
//		printf("thread %d, helpernextid = %d \n", omp_get_thread_num(), id);
			helperNextTask(id);
	}
}


bool helperWait(int id)
{
	while(helpers[id].totalIsCnt())
	{
	}
	return true;
}

bool helperLock(int id, pthread_rwlock_t *t)
{
	helpers[0].lockTry();
    int flag = pthread_rwlock_trywrlock(t);
	if(flag == 0){
		helperInit(id);
		helpers[0].unlockTry();
//		printf("thread %d, lock true\n", omp_get_thread_num());
		return true;
	}
	else{
		helpers[0].unlockTry();
//		printf("thread %d, lock false\n", omp_get_thread_num());
		helperWork(id);
		return false;
	}
}


bool helperNextTask(int id)
{
//	printf("thread %d, helperNextTask\n", omp_get_thread_num());

	HTask *t = helpers[id].First();
	if(t!=NULL)
	{
		t->Run();
		delete t;
		helpers[id].cntInc();
	}
};

void helperInit(int id)
{
	helpers[id].reset();
}


#endif
