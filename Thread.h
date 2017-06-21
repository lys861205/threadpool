#ifndef __thread_h__
#define __thread_h__
#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <error.h>
#include <string>

using namespace std;

typedef void (*ThreadFunc)();

class Thread
{
public:
	Thread(const ThreadFunc& cb, const string& name):
		started_(false),
		tid_(0),
		tcb_(cb),
		name_(name)
	{}
	~Thread()
	{
		
	}
	int start()
	{
		started_ = true;
		if ( pthread_create(&tid_, NULL, &Thread::threadFun, this ) < 0 ) 
		{
			printf("create thread failed, %d\n", errno);
			return -1;
		}
		return 0;
	}
	int join()
	{
		started_ = false;
		return pthread_join(tid_, NULL);
	}
private:
	static void* threadFun(void* p)
	{
		Thread* pt = static_cast<Thread*>(p);
		if ( pt ) 
		{
			pt->_threadFun();
		}
	}
	void _threadFun()
	{
		if (tcb_) 
			tcb_();
	}
private:
	bool started_;
	pthread_t tid_;
	ThreadFunc tcb_;
	string name_;
};


#endif //
