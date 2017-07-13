#ifndef __acceptor_h__
#define __acceptor_h__
#include "NetEvent.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <boost/function.hpp>
#include <boost/bind.hpp>

using namespace boost;

#define BACKLOG_SIZE	1024

void defaultNewSesssionCallback(int fd, int port, string sIp)
{
	cout << "new session ... " << endl;
}

class Acceptor : public NetEvent
{
	typedef boost::function<void(int, int, string)> NewSessionCallBack;
public:
	Acceptor(int fd):
		NetEvent(fd),
		newSession_(defaultNewSesssionCallback)
	{}
	~Acceptor()
	{}
	int handleEvent()
	{
		if ( revent_ & EV_ERROR )
			handlerError();
		else if ( revent_ & EV_READ )
			handleRead();
		else if ( revent_ & EV_WRITE )
			handleWrite();
		return 0;
	}
	int handleRead()
	{
		struct sockaddr_in addr;
		socklen_t len = sizeof(addr);
		int connfd = accept4(sockfd_,(struct sockaddr*)&addr, &len, SOCK_NONBLOCK);
		if ( connfd < 0 )
		{
			if ( errno == EAGAIN || errno == EWOULDBLOCK )
			{
				return 0;
			}
			else 
			{
				cout << "accept failed, error: " << errno << endl;
				return -1;
			}
		}
		string connIP(inet_ntoa(addr.sin_addr));
		int connPort = ntohs(addr.sin_port);
		newSession_(connfd, connPort, connIP);
		return 0;
	}
	int handleWrite()
	{
		return 0;
	}
	int handlerError()
	{
		return 0;
	}
	
	int startServer(int port, string& sIP)
	{
		struct sockaddr_in addr;
		addr.sin_family = AF_INET;
		addr.sin_port   = htons(port);
		addr.sin_addr.s_addr = inet_addr(sIP.c_str());
		int ret = ::bind(sockfd_, (const struct sockaddr*)&addr, sizeof(addr));
		if ( ret < 0 )
		{
			return ret;
		}
		ret = listen(sockfd_, BACKLOG_SIZE);
		if ( ret < 0 )
		{
			return ret;
		}
		return 0;
	}
	void setNewCallback(NewSessionCallBack cb)
	{
		newSession_ = cb;
	}

private:
	NewSessionCallBack newSession_;

};

#endif //__acceptor_h__ 
