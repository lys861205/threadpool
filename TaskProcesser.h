#ifndef  __TASKPROCESSER__H__
#define  __TASKPROCESSER__H__

#include <list>
#include <hash_set>
#include "define.h"
#include "TaskThread.h"
typedef void (__stdcall* WORKERPROC)(int nThreadNum, void* pTask, void* pUser);

template<class T>
class CTaskProcesser
{
	typedef std::list< T* > TList;
	typedef CTaskThread<CTaskProcesser> TaskThread;
	typedef std::list< TaskThread* > TaskThreadList;

	typedef enum{
		TASK_SUCCESS = 0,
		TASK_NORUN,
		TASK_FULL
	};
public:
	CTaskProcesser() : m_bRunning(false)
	{
	}

	~CTaskProcesser()
	{

	}

	bool Init(int nTaskNum, int nThreadNum)
	{
		for (int i=0; i<nTaskNum; i++)
		{
			T * pTask = new T();
			if (pTask)
			{
				m_hashTasks.insert(pTask);
				m_listFreeTasks.push_back(pTask);
			}
		}
		m_listWaitTasks.clear();

		for (int i=0; i<nThreadNum; i++)
		{
			TaskThread *pThread = new TaskThread(i, this);
			if (pThread)
			{
				m_listThreads.push_back(pThread);
				m_listThreadWaiters.push_back(pThread);
			}
		}
		m_nThreadNum = m_listThreads.size();
		m_nTaskNum   = m_hashTasks.size();
		return true;
	}

	void Cleanup()
	{
		Stop();

		for (TaskThreadList::iterator pos = m_listThreads.begin(); pos != m_listThreads.end(); ++pos)
		{
			delete *pos;
			*pos = NULL;
		}
		m_listThreads.clear();
		m_listThreadWaiters.clear();

		for (TList::iterator pos = m_hashTasks.begin(); pos != m_hashTasks.end(); ++pos)
		{
			delete *pos;
			*pos = NULL;
		}
		m_hashTasks.clear();
		m_listWaitTasks.clear();
		m_listFreeTasks.clear();
	}

	void Start(WORKERPROC func, void* user)
	{
		m_funTaskDeal = func;
		m_pUser = user;
		m_bRunning = true;
		for (TaskThreadList::iterator pos = m_listThreads.begin(); pos!= m_listThreads.end(); ++pos)
		{
			TaskThread *pThread = *pos;
			pThread->Start();
		}
	}

	void Stop()
	{
		m_bRunning = false;
		for (TaskThreadList::iterator pos = m_listThreads.begin(); pos!= m_listThreads.end(); ++pos)
		{
			TaskThread *pThread = *pos;
			pThread->Stop();
		}

		m_listFreeTasks.clear();
		for (TList::iterator pos = m_hashTasks.begin(); pos != m_hashTasks.end(); ++pos)
		{
			T* pTask = *pos;
			m_listFreeTasks.push_back(pTask);
		}
		m_listWaitTasks.clear();
	}

	T* GetTask()
	{
		m_criticalTaskWait.Lock();
		if (!m_listFreeTasks.size())
		{
			m_criticalTaskWait.UnLock();
			return NULL;
		}
		T* pTask = m_listFreeTasks.front();
		m_listFreeTasks.pop_front();
		m_criticalTaskWait.UnLock();
		return pTask;
	}

	void FreeTask(T* pTask)
	{
		if (m_hashTasks.find(pTask) == m_hashTasks.end()) 
		{
			return;
		}
		m_criticalTaskFree.Lock();
		m_listFreeTasks.push_back(pTask);
		m_criticalTaskFree.UnLock();
	}
	int Input(T* pInputTask)
	{
		if (!m_bRunning)
		{
			return TASK_NORUN;
		}
		T* pTask = pInputTask;
		{
			m_criticalTaskWait.Lock();
			m_listWaitTasks.push_back(pTask);
			NotifyNextThread(); //唤醒一个线程来执行任务
			m_criticalTaskWait.UnLock();
		}
		return TASK_SUCCESS;
	}

	void Worker(TaskThread *pThread)
	{
		T* pTask = NULL;
		{
			if (m_listWaitTasks.size())
			{
				m_criticalTaskWait.Lock();
				pTask = m_listWaitTasks.front();
				m_listWaitTasks.pop_front();
				if (m_listWaitTasks.size()) //还有任务需要执行
				{
					NotifyNextThread(); //唤醒下一个线程执行任务
				}
				m_criticalTaskWait.UnLock();
			}
			
		}
		if (pTask)
		{
			if (m_funTaskDeal)
			{
				m_funTaskDeal(pThread->m_nThreadNum, pTask, m_pUser);
			}
			return;
		}
		// 当无任务时，把线程存放到等待线程队列中，并使线程进入休眠状态
		JoinThreadWait(pThread);
		// 一直等待线程变为有信号状态，如果处于无信号状态，则表示线程是休眠状态
		pThread->m_evProcess.Wait();
	}

private:                 
	TList m_listFreeTasks;	             //空闲任务队列
	TList m_listWaitTasks;	             //等待处理的任务队列
	stdext::hash_set<T*> m_hashTasks;	 //执行器所管理的任务资源

	TaskThreadList m_listThreads;	         //线程队列
	UVPCritical m_criticalThread;
	TaskThreadList m_listThreadWaiters;       //等待唤醒事件的线程事件队列

	int m_nThreadNum;		             //执行线程的数量
	int m_nTaskNum;		                 //任务队列总数
	bool m_bRunning;
	UVPCritical m_criticalTaskWait;
	UVPCritical m_criticalTaskFree;


	WORKERPROC m_funTaskDeal;
	void *m_pUser;

	void NotifyNextThread()
	{		
		m_criticalThread.Lock();
		if (!m_listThreadWaiters.size())
		{
			m_criticalThread.UnLock();
			return;
		}
		TaskThread* pThread = m_listThreadWaiters.front();
		m_listThreadWaiters.pop_front();
		m_criticalThread.UnLock();
		pThread->m_evProcess.Set();
			
	}

	void JoinThreadWait(TaskThread * pThread)
	{

		pThread->m_evProcess.Reset();
		m_criticalThread.Lock();
		m_listThreadWaiters.push_back(pThread);
		m_criticalThread.UnLock();
	}
};
#endif