#ifndef _THTRAD_LOOP_H_
#define _THTRAD_LOOP_H_
#include "Thread.h"
#include "Mutex.h"
#include <list>
#include <vector>
using namespace std;

typedef void(*Function)(void);

class ThreadLoop
{
public:
	ThreadLoop():
		quit_(false),
		tid_(CurrentThread::tid())
	{
		
	}
	~ThreadLoop()
	{
		quit_ = true;
	}
	void loop()
	{
		while(!quit_)
		{
//			printf("xxxxxxxxxxxx\n");
			sleep(2);
			doFunctionEvent();
		}
	}
	void quit()
	{
		quit_ = true;
	}
	void doFunctionEvent()
	{
		vector<Function> tmpFunctionVec;
		{
			MUTEX_LOCK(mutex_);
			printf("===>%p, %p, %d\n", &mutex_, this, __LINE__);
			tmpFunctionVec.swap(vecFunction_);
		}
		for ( int i=0; i< tmpFunctionVec.size(); ++i )
		{
			tmpFunctionVec[i]();
		}
	}
	void postFunction(const Function& func)
	{
		if ( isCurrentThread() )
		{
			func();
		}
		else 
		{
			MUTEX_LOCK(mutex_);
			printf("===>%p, %p, %d\n", &mutex_, this, __LINE__);
			vecFunction_.push_back(func);
		}
	}
private:
	bool isCurrentThread()
	{
		return tid_ == CurrentThread::tid();
	}
private:
	bool quit_;
	pid_t tid_;
	vector<Function> vecFunction_;
	Mutex mutex_;
};

#endif //
