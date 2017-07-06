#ifndef _THTRAD_LOOP_H_
#define _THTRAD_LOOP_H_
#include "Thread.h"
#include "Mutex.h"
#include <list>
#include <vector>
#include "poller.h"
#include "epoll.h"
using namespace std;


int createEventfd()
{
  int evtfd = ::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
  if (evtfd < 0)
  {
    LOG_SYSERR << "Failed in eventfd";
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
		wakeupEvent_(new TcpEvent(createEventfd()),
		needwakeup_(true)
	{
		assert(poller_);
		assert(wakeupEvent_);
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
			int ret = poller_.poll(10000, activeEventList_);
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
			TcpEventListIter it;
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
		TcpEventList tmpPendingEvent;
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
	void postEvent(TcpEvent* pEvent)
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
	TcpEventList activeEventList_;
	TcpEventList pendingEventList_;
	TcpEvent* wakeupEvent_;
	bool needwakeup_;
};

#endif //
