#include "ThreadLoop.h"
#include "TcpServer.h"
#include <stdio.h>


int main(int argc, char** argv)
{
	if ( argc < 2 )
	{
		cout << "option: ip" << endl;
		return 0;
	}
	string sIP(argv[1]);
    ThreadLoop loop;
	TcpServer server(&loop, 2009, sIP);
	server.start(0);
    loop.loop();
    return 0;
}
