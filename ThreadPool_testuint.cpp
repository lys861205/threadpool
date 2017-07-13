#include "ThreadPool.h"
#include "ThreadLoop.h"
#include ".TcpServer.h"
#include "Thread.h"
#include <stdio.h>

void func()
{
    printf("========%lu\n", CurrentThread::tid());
    sleep(1);
}

int main()
{
    ThreadLoop loop;
    ThreadPool pool(&loop, 3);
    pool.start();
    ThreadLoop* lp = pool.getNextLoop();
    lp->postFunction(func);

    ThreadLoop* lp1 = pool.getNextLoop();
    lp1->postFunction(func);

    loop.loop();
    
    return 0;
}
