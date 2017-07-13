#ifndef __tcp_server_h__
#define __tcp_server_h__

#include "ThreadLoop.h"
#include "ThreadPool.h"
#include "Acceptor.h"
#include "Connection.h"
#include "SocketUtility.h"
#include <iostream>
#include <string>
#include <vector>
#include <memory.h>
using namespace std;

// typedef vector<Connection*>

class TcpServer
{
public:
	TcpServer(ThreadLoop* l, int port, string& sIP):
		loop_(l),
		pool_(l),
		acceptor_(SocketUtil::createSocket(1))
	{
		if ( acceptor_.startServer(port, sIP) < 0 )
		{
			cout << "create server failed, error: " << errno << endl;
			abort();
		}
		acceptor_.enableRead();
		acceptor_.setNewCallback(boost::bind(&TcpServer::newSession,this,_1,_2,_3));	
	}
	void start(int threadNum)
	{
		loop_->postEvent(&acceptor_);
		pool_.start(threadNum);	
	}

	void newSession(int fd, int port, string sIP)
	{
		ThreadLoop* ioLoop = pool_.getNextLoop();
		printf("[%d] connection, info[%s,%d]\n", fd, sIP.c_str(), port);
		Connection* conn(new Connection(fd,ioLoop));
		conn->enableRead();
		ioLoop->postEvent(conn);
	}

private:
	ThreadLoop* loop_;
	ThreadPool pool_;	
	Acceptor acceptor_;

};


#endif //
