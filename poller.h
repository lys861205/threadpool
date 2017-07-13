#ifndef __POLLER_H__
#define __POLLER_H__

#include <vector>
using namespace std;

class NetEvent;

typedef vector<NetEvent*> NetEventList;
typedef vector<NetEvent*>::iterator NetEventListIter;


class Poller
{
public:
	Poller(){}
	virtual ~Poller(){}
	virtual void updateEvent(NetEvent* pEvent)
	{
	}
	virtual void update(int op, NetEvent* pEvent)
	{
	}
	virtual int poll(int timeoutMs, NetEventList& rOutEventList){}
private:
	NetEventList tcpConnectVec_;
	
};

#endif // __POLLER_H__
