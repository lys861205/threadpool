#include "SocketUtility.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>

int SocketUtil::createSocket(int proto)
{
	return socket(AF_INET, SOCK_STREAM, 0);
}
int SocketUtil::createNonBlockSocket(int proto)
{
	int fd = socket(AF_INET, SOCK_STREAM, 0);
	if ( fd < 0 )
	{
		abort();
	}
	setReuse(fd);
	setNonBlock(fd);
	return fd;
}
int SocketUtil::setNonBlock(int fd)
{
	int old = fcntl(fd, F_GETFD, 0);
	old |= O_NONBLOCK;
	return ::fcntl(fd, F_SETFD, old);
}


int SocketUtil::setReuse(int fd)
{
	int reuse = 1;
	return setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));
}
