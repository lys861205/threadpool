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
			NotifyNextThread(); //����һ���߳���ִ������
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
				if (m_listWaitTasks.size()) //����������Ҫִ��
				{
					NotifyNextThread(); //������һ���߳�ִ������
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
		// ��������ʱ�����̴߳�ŵ��ȴ��̶߳����У���ʹ�߳̽�������״̬
		JoinThreadWait(pThread);
		// һֱ�ȴ��̱߳�Ϊ���ź�״̬������������ź�״̬�����ʾ�߳�������״̬
		pThread->m_evProcess.Wait();
	}

private:                 
	TList m_listFreeTasks;	             //�����������
	TList m_listWaitTasks;	             //�ȴ�������������
	stdext::hash_set<T*> m_hashTasks;	 //ִ�����������������Դ

	TaskThreadList m_listThreads;	         //�̶߳���
	UVPCritical m_criticalThread;
	TaskThreadList m_listThreadWaiters;       //�ȴ������¼����߳��¼�����

	int m_nThreadNum;		             //ִ���̵߳�����
	int m_nTaskNum;		                 //�����������
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