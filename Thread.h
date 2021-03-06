#ifndef __thread_h__
#define __thread_h__
#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <string>
#include <sys/syscall.h>

using namespace std;


namespace CurrentThread
{
	pid_t tid()
	{
		return syscall(__NR_gettid);
	}
	bool isMainThread()
	{
		return tid() == getpid();
	}
}

typedef void (*ThreadFunc)(void*);

class Thread
{
public:
	Thread(const ThreadFunc& cb, void*arg, const string& name):
		started_(false),
		tid_(0),
		tcb_(cb),
        arg_(arg),
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
			tcb_(arg_);
	}
private:
	bool started_;
	pthread_t tid_;
	ThreadFunc tcb_;
    void* arg_;
	string name_;
};


#endif //
