#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <memory>
#include <deque>
#include <list>
#include <vector>
#include <sys/syscall.h>
using namespace std;

#define gettid() syscall(__NR_gettid)

class Follower
{
public:
	Follower()
	{
		pthread_mutex_init(&_mutex, NULL);
		pthread_cond_init(&_cond, NULL);
	}
	~Follower()
	{
		pthread_mutex_destroy(&_mutex);
		pthread_cond_destroy(&_cond);
	}
	int wait()
	{
		return pthread_cond_wait(&_cond, &_mutex);
	}
	int notify()
	{
		return pthread_cond_signal(&_cond);
	}
private:
	pthread_mutex_t _mutex;
	pthread_cond_t _cond;
};

template<typename T>
class LFThreadPool
{
public:
	LFThreadPool()
	{
		pthread_mutex_init(&_followLock, NULL);
	}
	~LFThreadPool()
	{
		pthread_mutex_destroy(&_followLock);
	}
	int leaderActive()
	{
		 return _curLeaderId != 0;
	}
	void leaderActive(pthread_t tid)
	{
		_curLeaderId = tid;
	}
	Follower* makeFollower()
	{
		//shared_ptr<Follower*> pFollower(new Follower());
		Follower* pFollower = new Follower();
		pthread_mutex_lock(&_followLock);
		_followers.push_back(pFollower);
		pthread_mutex_unlock(&_followLock);
		return pFollower;
	}
	int becomeLeader()
	{
		if ( leaderActive () )
		{
			Follower* fw = makeFollower();
			while( leaderActive() )
				fw->wait();
			delete fw;
		}
		leaderActive(10);
		return 0;

	}
	int selectLeader()
	{
		leaderActive(0);
		if ( !_followers.empty() )
		{
			Follower* fw = _followers.front();
			_followers.pop_front();
			return fw->notify();
		}
		return -1;
	}
	int spawn(int size)
	{
		size = size > 0 ? size:1;
		pthread_t t;
		int ret = 0;
		for ( int i=0; i<size; ++i )
		{
			ret = pthread_create(&t, NULL, LFThreadPool::threadFunc, this);
			if ( ret != 0 )
			{
				printf(" create thread  %d, failed, error: %d\n", i, errno);
				continue;
			}
			printf("-----> %ld\n", t);
			_threadList.push_back(t);
		}
	}
	static void* threadFunc(void* p)
	{
		LFThreadPool* pool = static_cast<LFThreadPool*>(p);
		if ( pool )
		{
			pool->_threadFunc();
		}
	}
private:
	int _threadFunc()
	{
		while( 1 )
		{
			becomeLeader();
			selectLeader();
			printf("do message: %d\n", gettid());
			sleep(1);
		}
		return 0;
	}

private:
	pthread_t _curLeaderId;
	pthread_mutex_t _followLock;
	deque<Follower*> _followers;
	deque<T*> _msgs;
	list<pthread_t> _threadList;
};



int main()
{
	LFThreadPool<int> p;

	p.spawn(5);
	while(1)
		sleep(1);
	return 0;



}
