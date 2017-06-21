#ifndef __THREAD_POOL_H__
#define __THREAD_POOL_H__

#include <vector>
#include "ThreadLoop.h"
#include "Thread.h"
using namespace std;

class EventThreadLoop{
public:
	EventThreadLoop(const string& name):
		loop_(NULL),
		thread_(new Thread(threadFun, name))
	{}
	~EventThreadLoop()
	{
		thread_.join();
	}
	ThreadLoop* getLoop()
	{
		thread_.start();
		lock(mutex);
		while(loop_==NULL)
			cond_.wait();
		return loop_;
	}
	void threadFun()
	{
		ThreadLoop loop;
		{
			lock(mutex)
			loop_ = &loop;
			cond.notify();
		}
		loop.loop();
		loop_ = NULL;
	}
private:
	ThreadLoop* loop_;
	Thread* thread_;
	LockCond cond_;
};

class ThreadPool
{
public:
	ThreadPool(int num=2):
		loop_(NULL),
		numThreads_(num)
	{}

	~ThreadPool()
	{
	}
	void start()
	{
		for ( int i=0; i<numThreads_; ++i )
		{
			char buf[32] = {0};
			sprintf(buf, "thread:%d", i);
			EventThreadLoop* t = new EventThreadLoop(string(buf));
			vecThread_.push_back(t);
			vecLoop_.push_back(t.getLoop());
		}
	}
private:
	ThreadLoop* loop_;
	vector<Thread*> vecThread_;
	vector<ThreadLoop> vecLoop_;
	int numThreads_ 
};

#endif //
