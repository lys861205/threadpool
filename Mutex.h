#ifndef __MUTEX_H__
#define __MUTEX_H__
#include <pthread.h>
#include <time.h>
#include <unistd.h>
class Mutex
{
public:
	Mutex()
	{
		pthread_mutex_init(&mutex_, NULL);
	}
	virtual ~Mutex()
	{
		pthread_mutex_destroy(&mutex_);
	}
	int lock()
	{
		return pthread_mutex_lock(&mutex_);
	}
	int unLock()
	{
		return pthread_mutex_unlock(&mutex_);
	}
	
	pthread_mutex_t* getMutexT() 
	{
		return &mutex_;
	}
private:
	//Mutex(const Mutex& rh);
	//Mutex& operator=(const Mutex& r);
private:
	pthread_mutex_t mutex_;
};

class LockMutex
{
public:
	LockMutex(Mutex& m):
	cMutex_(m)
	{
		lock();
	}
	virtual ~LockMutex()
	{
		unLock();
	}
	int lock()
	{
		return cMutex_.lock();
	}
	int unLock()
	{
		return cMutex_.unLock();
	}
private:
	Mutex& cMutex_;
};

class LockCond
{
public:
	LockCond(Mutex& m):
	cMutex_(m)
	{
		pthread_cond_init(&cond_, NULL);
	}
	~LockCond()
	{
		pthread_cond_destroy(&cond_);
	}
	int wait()
	{
		return pthread_cond_wait(&cond_, cMutex_.getMutexT());
	}
	int tryWait(int seconds)
	{
		struct timespec timeout;
		timeout.tv_sec = seconds + time(NULL);
		timeout.tv_nsec= 0;
		return pthread_cond_timedwait(&cond_, cMutex_.getMutexT(), &timeout);
	}
	int notify()
	{
		return pthread_cond_signal(&cond_);
	}
private:
	Mutex& cMutex_;
	pthread_cond_t cond_;
};

class RWMutex
{
public:
	RWMutex()
	{
		pthread_rwlock_init(&rwMutex_, NULL);
	}
	virtual ~RWMutex()
	{
		pthread_rwlock_destroy(&rwMutex_);
	}
	int rLock()
	{
		return pthread_rwlock_rdlock(&rwMutex_);
	}
	int wLock()
	{
		return pthread_rwlock_wrlock(&rwMutex_);
	}
	int unLock()
	{
		return pthread_rwlock_unlock(&rwMutex_);
	}
private:
	pthread_rwlock_t rwMutex_;
};

class LockRWMutex
{
public:
	LockRWMutex(const RWMutex& rw, bool bRead):
	cRwMutex_(rw)
	{
		if (bRead)
		{
			cRwMutex_.rLock();
		}
		else 
		{
			cRwMutex_.wLock();
		}
	}
	virtual ~LockRWMutex()
	{
		cRwMutex_.unLock();
	}
private:
	RWMutex cRwMutex_;
};

#define MUTEX_LOCK(m) LockMutex l(m)
#define READ_LOCK(m)  LockRWMutex l(m, true)
#define WRITE_LOCK(m) LockRWMutex l(m, false)

#endif //__MUTEX_H__
