// 2008-02-23 17:06
// UVPThread.cpp
// guosh
// 线程类实现

#include "UVPThread.h"

namespace uvp {

///////////////////////////////////////////////////////////////////////////////
// UVPThread
////////////////////////////////////////////////////////////////////////////////
#ifdef OS_WINDOWS
UVPThread::UVPThread()
	: m_hThread(NULL)
	, m_uThreadID(0)
	, m_pfThreadProc(NULL)
	, m_pParam(NULL)
	, m_evQuit(TRUE)
{
	// empty
}

UVPThread::~UVPThread()
{
	//ASSERT(!m_uThreadID && "Need to hand call Join before UVPThread destruction.");
}

BOOL UVPThread::Start(PFTHREADPROC pfThreadProc, void *pParam)
{
	Join();

	if (pfThreadProc == NULL)
	{
		m_pfThreadProc = UVPThread::_Entry;
		m_pParam = this;
	}
	else
	{
		m_pfThreadProc = pfThreadProc;
		m_pParam = pParam;
	}

	unsigned threadID;
	m_hThread = (HANDLE)_beginthreadex(NULL, 0, m_pfThreadProc, m_pParam, 0, &threadID);
	if (m_hThread == NULL)
	{
		return FALSE;
	}
	m_uThreadID = (uint32)threadID;

	return TRUE;
}

// 检测退出信号
BOOL UVPThread::TryWaitQuit(uint32 uMilliseconds)
{
	return m_evQuit.TryWait(uMilliseconds);
}

void UVPThread::PostStop()
{
	m_evQuit.Set();
	return;
}

void UVPThread::Exit()
{
	_endthreadex(0);
	return;
}

void UVPThread::Kill()
{
	TerminateThread(m_hThread, 0);
	return;
}

void UVPThread::Join()
{
	if (m_uThreadID == 0) return;

	WaitForSingleObject(m_hThread, INFINITE);
	CloseHandle(m_hThread);
	m_hThread = NULL;

	m_uThreadID = 0;
	m_evQuit.Reset();
	return;
}

unsigned UVPThread::_Entry(void* pParam)
{
	UVPThread *pThis = static_cast<UVPThread *>(pParam);
	if (pThis)
	{
		pThis->Entry();
	}
	return 0;
}

// 线程休眠指定毫秒值
void UVPThread::Sleep(uint32 nMillisecond)
{
	::Sleep(nMillisecond);
	return;
}

// 获取线程自身ID
uint32 UVPThread::SelfID(void)
{
	return (uint32)GetCurrentThreadId();
}
#endif//OS_WINDOWS


///////////////////////////////////////////////////////////////////////////////
// UVPThread
////////////////////////////////////////////////////////////////////////////////
#ifdef OS_LINUX
UVPThread::UVPThread()
	: m_uThreadID(0)
	, m_pfThreadProc(NULL)
	, m_pParam(NULL)
	, m_evQuit(TRUE)
{
	// empty
}

UVPThread::~UVPThread()
{
	ASSERT(!m_uThreadID && "Need to hand call Join before UVPThread destruction.");
}

BOOL UVPThread::Start(PFTHREADPROC pfThreadProc, void *pParam)
{
	Join();

	if (pfThreadProc == NULL)
	{
		m_pfThreadProc = UVPThread::_Entry;
		m_pParam = this;
	}
	else
	{
		m_pfThreadProc = pfThreadProc;
		m_pParam = pParam;
	}

	pthread_t threadID = 0;
	if (0 != pthread_create(&threadID, NULL, m_pfThreadProc, m_pParam))
	{
		return FALSE;
	}
	m_uThreadID = (uint32)threadID;

	return TRUE;
}

// 检测退出信号
BOOL UVPThread::TryWaitQuit(uint32 uMilliseconds)
{
	return m_evQuit.TryWait(uMilliseconds);
}

void UVPThread::PostStop()
{
	m_evQuit.Set();
	return;
}

void UVPThread::Exit()
{
	pthread_exit(0);
	return;
}

void UVPThread::Kill()
{
	pthread_cancel((pthread_t)m_uThreadID);
	return;
}

void UVPThread::Join()
{
	if (m_uThreadID == 0) return;

	pthread_join(m_uThreadID, NULL);

	m_uThreadID = 0;
	m_evQuit.Reset();
	return;
}

void* UVPThread::_Entry(void *pParam)
{
	UVPThread *pThis = static_cast<UVPThread *>(pParam);
	if (pThis)
	{
		pThis->Entry();
	}
	return 0;
}


// 线程休眠指定毫秒值
void UVPThread::Sleep(uint32 nMillisecond)
{
	struct timeval tv;
	tv.tv_sec = nMillisecond / 1000;
	tv.tv_usec = (nMillisecond % 1000) * 1000;
	select(0, NULL, NULL, NULL, &tv);
	return;
}

// 获取线程自身ID
uint32 UVPThread::SelfID(void)
{
	return (uint32)pthread_self();
}
#endif//OS_LINUX


//  线程是否退出
BOOL UVPThread::IsExitThread()
{
	return TryWaitQuit();
}

void UVPThread::Entry()
{
	return;
};

} // namespace uvp

