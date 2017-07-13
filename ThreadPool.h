#ifndef __THREAD_POOL_H__
#define __THREAD_POOL_H__

#include <vector>
#include <string>
#include "ThreadLoop.h"
#include "Thread.h"
#include "Mutex.h"
using namespace std;

class EventThreadLoop{
public:
	EventThreadLoop(const string& name):
		loop_(NULL),
		thread_(new Thread(&EventThreadLoop::threadFun,this,  name)),
		cond_(mutex_)
	{}
	~EventThreadLoop()
	{
		thread_->join();
	}
	ThreadLoop* getLoop()
	{
		{
			MUTEX_LOCK(mutex_);
		    thread_->start();
			while(loop_==NULL)
            {
				cond_.wait();
            }
		}
		return loop_;
	}
	static void threadFun(void* p)
	{
       EventThreadLoop* loop = static_cast<EventThreadLoop*>(p);
       loop->_threadFun();
	}
    void _threadFun()
    {
    	ThreadLoop loop;
		{
			MUTEX_LOCK(mutex_);
			loop_ = &loop;
			cond_.notify();
		}
		loop.loop();
		loop_ = NULL;
    }
private:
	ThreadLoop* loop_;
	Thread* thread_;
	Mutex mutex_;
	LockCond cond_;
	
};

class ThreadPool
{
public:
	ThreadPool(ThreadLoop* loop, int num=0):
		loop_(loop),
		numThreads_(num),
		next_(0)
	{}

	~ThreadPool()
	{
		for ( int i=0; i<vecThread_.size(); ++i )
		{
			delete vecThread_[i];
		}
		vecThread_.clear();
	}
	void start(int n = 2)
	{
		numThreads_ = numThreads_ > n ? numThreads_: n;
		for ( int i=0; i<numThreads_; ++i )
		{
			char buf[32] = {0};
			sprintf(buf, "thread:%d", i);
			EventThreadLoop* t = new EventThreadLoop(buf);
			vecThread_.push_back(t);
			vecLoop_.push_back((t->getLoop()));
		}
	}
	ThreadLoop* getNextLoop()
	{
		ThreadLoop* l;
		if ( vecLoop_.empty() )
		{
			return loop_;
		}
		if ( next_ >= vecLoop_.size() )
		{
			next_ = 0;
		}
		l = vecLoop_[next_++];
		return l;
	}
private:
	ThreadLoop* loop_;
	vector<EventThreadLoop*> vecThread_;
	vector<ThreadLoop*> vecLoop_;
	int next_;
	int numThreads_ ;
};

#endif //
