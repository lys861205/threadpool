#ifndef __SOCKET_H__
#define __SOCKET_H__

#include <string>
using namespace std;

namespace net
{


class Socket
{
public:
	enum
	{
		kUDP=1,
		kTCP,
	};
public:
	static int create(bool reuse=true, int stream=kTCP);
	static int createNonBlock(bool reuse=true, int stream=kTCP);
	static int setNonBlock(int fd);
	static int setReuseAddr(int fd, int on);
	static int setReusePort(int fd, int on);
	static int setKeepAlive(int fd, int on);
	static int bindAndListen(int fd, string& sIp, short port);
	static int acceptAndSetNonBlock(int fd, string& rIp, short& rPort);
	static int accept(int fd, string& rIp, short& rPort);
	static int recv(int fd,  void* rBuf, int rsize);
	static int send(int fd, void* sBuf, int ssize);
	static int close(int fd);
};

};
using namespace net;

#endif // __SOCKET_H__