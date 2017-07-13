#include "Socket.h"
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/ioctl.h> 
#include <arpa/inet.h>
#include <sys/fcntl.h>



#define MAXCONN	1024


int Socket::create(bool reuse, int stream)
{
	int type=(stream==kTCP) ? SOCK_STREAM : SOCK_DGRAM;
	int fd = socket(AF_INET, type, 0);
	if ( fd < 0 ) 
		return fd;
	int on = reuse ? 1 : 0;
	if ( setReuseAddr(fd, on) < 0 )
	{
		return -1;
	}
	return fd;
}
int Socket::createNonBlock(bool reuse, int stream)
{
	int type=(stream==kTCP) ? SOCK_STREAM : SOCK_DGRAM;
	int fd = socket(AF_INET, type, 0);
	if ( fd < 0 )
		return fd;
	int on = reuse ? 1 : 0;
	if ( setReuseAddr(fd, on) < 0 )
		return -1;
	if ( setNonBlock(fd) < 0 )
		return -1;
	return fd;
}
int Socket::setNonBlock(int fd)
{
	int old = ::fcntl(fd, F_GETFL, 0);
	old |= O_NONBLOCK;
	return ::fcntl(fd, F_SETFL, old);
}
int Socket::setReuseAddr(int fd, int on)
{
	return ::setsockopt(fd, SOL_SOCKET,SO_REUSEADDR, &on ,sizeof(on));
}
int Socket::setReusePort(int fd, int on)
{
	return ::setsockopt(fd, SOL_SOCKET,SO_REUSEPORT, &on ,sizeof(on));
}
int Socket::setKeepAlive(int fd, int on)
{
	return ::setsockopt(fd, SOL_SOCKET,SO_KEEPALIVE, &on ,sizeof(on));
}
int Socket::bindAndListen(int fd, string& sIp, short port)
{
	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port   = htons(port);
	addr.sin_addr.s_addr = inet_addr(sIp.c_str());
	int ret = ::bind(fd, (struct sockaddr*)&addr, sizeof(addr));
	if ( ret < 0 )
		return ret;
	ret = ::listen(fd, MAXCONN);
	if ( ret < 0 )
		return ret;
	return 0;
}
int Socket::acceptAndSetNonBlock(int fd, string& rIp, short& rPort)
{
	int sockfd = accept(fd, rIp, rPort);
	if ( sockfd < 0 )
		return -1;
	
	setNonBlock(sockfd);
	return sockfd;
}
int Socket::accept(int fd, string& rIp, short& rPort)
{
	struct sockaddr_in raddr;
	socklen_t addrlen = sizeof(raddr);
	int sockfd = ::accept(fd, (struct sockaddr*)&raddr, &addrlen);
	if ( sockfd < 0 )
		return -1;
	rIp 	= inet_ntoa(raddr.sin_addr);
	rPort 	= ntohs(raddr.sin_port);
	return sockfd;
}
int Socket::recv(int fd, void* rBuf, int rsize)
{
	return ::recv(fd, rBuf, rsize, 0);
}
int Socket::send(int fd,  void* sBuf, int ssize)
{
	return ::send(fd, sBuf, ssize, 0);
}
int Socket::close(int fd)
{
	if ( fd > 0 )
		close(fd);
}
