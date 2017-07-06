#ifndef __POLLER_H__
#define __POLLER_H__

#include <vector>
using namespace std;

class TcpEvent;

typedef vector<TcpEvent*> TcpEventList;


class Poller
{
public:
	Poller();
	virtual ~Poller();
	void updateEvent(TcpEvent* pEvent)
	{
	}
	void update(int op, TcpEvent* pEvent)
	{
	}
	virtual int poll(int timeoutMs, TcpEventList& rOutEventList);
private:
	TcpEventList tcpConnectVec_;
	
};

#endif // __POLLER_H__