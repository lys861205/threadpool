#ifndef _THTRAD_LOOP_H_
#define _THTRAD_LOOP_H_
#include "Thread.h"
#include "Mutex.h"
#include <list>
#include <vector>
#include <sys/eventfd.h>
#include <unistd.h>
#include <iostream>
#include "poller.h"
#include "epoll.h"
using namespace std;


int createEventfd()
{
  int evtfd = ::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
  if (evtfd < 0)
  {
    cout  << "Failed in eventfd" << endl;;
    abort();
  }
  return evtfd;
}

class ThreadLoop
{
public:
	ThreadLoop():
		quit_(false),
		tid_(CurrentThread::tid()),
		poller_(new EPoller()),
		wakeupEvent_(new NetEvent(createEventfd())),
		needwakeup_(true)
	{
		assert(poller_);
		assert(wakeupEvent_);
		wakeupEvent_->enableRead();
		poller_->updateEvent(wakeupEvent_);
	}
	~ThreadLoop()
	{
		quit_ = true;
		if ( poller_ )
		{
			delete poller_;
			poller_ = NULL;
		}
		if ( wakeupEvent_ )
		{
			delete wakeupEvent_;
			wakeupEvent_ = NULL;
		}
	}
	void loop()
	{
		while(!quit_)
		{
			activeEventList_.clear();
			int ret = poller_->poll(10000, activeEventList_);
			if ( ret == 0 )
			{
				continue;
			}
			if ( ret < 0 )
			{	
				printf("fatal error!\n");
				continue;
			}
			needwakeup_ = false;
			NetEventListIter it;
			for ( it=activeEventList_.begin(); it != activeEventList_.end(); ++it )
			{
				(*it)->handleEvent();
			}
			doPendingEvent();
		}
	}
	void quit()
	{
		quit_ = true;
	}
	void doPendingEvent()
	{
		NetEventList tmpPendingEvent;
		{
			MUTEX_LOCK(mutex_);
			tmpPendingEvent.swap(pendingEventList_);
		}
		needwakeup_ = true;
		for ( int i=0; i< tmpPendingEvent.size(); ++i )
		{
			poller_->updateEvent(tmpPendingEvent[i]);
		}
	}
	void postEvent(NetEvent* pEvent)
	{
		if ( isCurrentThread() )
		{
			poller_->updateEvent(pEvent);
		}
		else 
		{
			MUTEX_LOCK(mutex_);
			pendingEventList_.push_back(pEvent);
			if ( needwakeup_ )
			{
				wakeup();
			}
		}
	}
private:
	void wakeup()
	{
		uint64_t one = 1;
		ssize_t n = write(wakeupEvent_->socket(), &one, sizeof one);
		if ( n != sizeof one )
		{
			cout << "wakeup write " << n << " bytes" << endl;
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
	Mutex mutex_;
	Poller*	poller_;
	NetEventList activeEventList_;
	NetEventList pendingEventList_;
	NetEvent* wakeupEvent_;
	bool needwakeup_;
};

#endif //
