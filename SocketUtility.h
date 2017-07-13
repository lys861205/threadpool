#ifndef __SOCKET_UTILITY_H__
#define __SOCKET_UTILITY_H__

class SocketUtil
{
public:
	static int createSocket(int proto);
	static int createNonBlockSocket(int proto);
	static int setNonBlock(int fd);
	static int setReuse(int fd);
};

#endif //__SOCKET_UTILITY_H__
