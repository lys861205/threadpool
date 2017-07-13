#ifndef __CONNECTION_H__
#define __CONNECTION_H__

#include "NetEvent.h"
#include "ThreadLoop.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>

class Connection : public NetEvent
{
public:
	Connection(int sockfd, ThreadLoop* l):
		NetEvent(sockfd),
		ioLoop_(l)
	{}
	~Connection()
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
		memset(echoBuf, 0x00, 1024);
		int ret = recv(sockfd_,echoBuf, 1023, 0);
		if ( ret <= 0 )
		{
			if ( errno == EAGAIN || errno == EWOULDBLOCK ) 
			{
				return 0;
			}
			setNoneEvent();
			close(sockfd_);
			ioLoop_->postEvent(this);
			return -1;
		}
		enableWrite();	
		ioLoop_->postEvent(this);

		cout << "recv message: " << echoBuf << endl;
		return 0;
	}
	int handleWrite()
	{
		cout << "send message: " << echoBuf << endl;
		int ret = send(sockfd_, echoBuf, strlen(echoBuf), 0);	
		if ( ret < 0 )
		{
			if ( errno == EAGAIN || errno == EWOULDBLOCK ) 
			{
				return 0;
			}
			setNoneEvent();
			close(sockfd_);
			ioLoop_->postEvent(this);
			return -1;
		}
		disableWrite();
		ioLoop_->postEvent(this);
		return 0;
	}
	int handlerError()
	{
		return 0;
	}
private:
	ThreadLoop* ioLoop_;
	char echoBuf[1024];
			
			
	
};

#endif //__CONNECTION_H__
