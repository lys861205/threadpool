#ifndef __epoll_h__
#define __epoll_h__

#include <vector>
using namespace std;

#define EPOLL_MAX 				256
#define EPOLL_EVENT_INIT_SIZE	1024

struct epoll_event;

class EPoller : public Poller
{
	typedef vector<struct epoll_event> TEventList;
private:
	static const int kNew = 0;
	static const int kAdd = 1;
	static const int kDelete = 2;
public:
	EPoller():
	epollfd_(create_epoll(EPOLL_MAX)),
	eventVec_(EPOLL_EVENT_INIT_SIZE)
	{
		if ( epollfd_ < 0 )
		{
			printf("create_epoll failed, error:%d\n", errno);
			exit(1);
		}
	}
	~EPoller()
	{
	}
	void updateEvent(NetEvent* pEvent)
	{
		assert(pEvent);
		int op = pEvent->OpFlag();
		if ( op == kNew )
		{
			pEvent->setOpFlag(kAdd);
			update(EPOLL_CTL_ADD, pEvent);
		}
		else 
		{
			if ( pEvent->isNoneEvent())
			{
				pEvent->setOpFlag(kDelete);
				update(EPOLL_CTL_DEL, pEvent);
			}
			else 
			{
				update(EPOLL_CTL_MOD, pEvent);
			}
		}
	}
	void update(int op, NetEvent* pEvent)
	{
		struct epoll_event event;
		event.data.ptr = pEvent;
		event.events = pEvent->event();
		::epoll_ctl(epollfd_, op, pEvent->socket(), &event);
	}
	int poll(int timeoutMs, NetEventList& rOutEventList)
	{
		int numEvents = ::epoll_wait(epollfd_, &eventVec_[0], 
			static_cast<int>(eventVec_.size(),
			timeoutMs);
		if ( numEvents == 0 )
		{
			//timeout
		}
		else if ( numEvents < 0 )
		{
			if ( errno != EINTR )
			{
				printf("epoll_wait failed, error: %d\n", errno);
				return -1;
			}
			return 0;
		}
		else 
		{
			for ( int i=0; i < numEvents; ++i )
			{
				struct epoll_event& rev = eventVec_[i];
				NetEvent* tcpEv = rev.data.ptr;
				tcpEv->set_revent(rev.events);
				rOutEventList.push_back(tcpEv);
			}
		}
		return numEvents;
	}
private:
	typedef
	int epollfd_;
	TEventList eventVec_;
};

#endif //__epoll_h__