#ifndef __Tcp_Event_H__
#define __Tcp_Event_H__

#define EV_NONE		0x00
#define	EV_READ		EPOLLIN|EPOLLERR
#define EV_WRITE	EPOLLOUT|EPOLLERR

class NetEvent
{
public:
	NetEvent(int fd):
	event_(EV_NONE),
	sockfd_(fd),
	revent_(EV_NONE),
	oper_(0)
	{
	}
	virtual ~NetEvent()
	{
		close(sockfd_);
	}
	virtual int handleEvent()
	{
		if ( revent_ & EV_READ )
		{
			uint64_t u;
			ssize_t n = read(sockfd_, &u ,sizeof(u));
			if ( n != sizeof(u) )
			{
				cout << "weakup read " << n " bytes." << endl;
			}
		}
		return 0;
	}
	virtual int handleRead()
	{
		return 0;
	}
	virtual int handleWrite()
	{
		return 0;
	}
	virtual int handlerError()
	{
		return 0;
	}
	virtual void enableRead()
	{
		event_ |= EV_READ;
	}
	virtual void enableWrite()
	{
		event_ |= EV_WRITE;
	}
	int event() const
	{
		return event_;
	}
	bool isNoneEvent()
	{
		return event_ == EV_NONE;
	}
	int socket() const 
	{
		return sockfd_;
	}
	void set_revent(int event)
	{
		revent_ = event;
	}
	void setOpFlag(int op)
	{
		oper_ = op;
	}
	int OpFlag()const 
	{
		return oper_;
	}
private:
	int event_;
	int sockfd_;
	int revent_;
	int oper_;
};

#endif //