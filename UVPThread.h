// 2008-02-23 17:06
// UVPThread.h
// guosh
// �߳���ʵ��


#pragma once

#ifndef _UVP_THREAD_H_
#define _UVP_THREAD_H_

#include "Define.h"
#include "UVPEvent.h"
#include "UVPNoncopyable.h"

namespace uvp {

///////////////////////////////////////////////////////////////////////////////
// UVPThread
////////////////////////////////////////////////////////////////////////////////
class  UVPThread : UVPNonCopyable
{
public:
	UVPThread();

	virtual ~UVPThread();

public:
	virtual BOOL Start(PFTHREADPROC pfThreadProc = NULL, void *pParam = NULL);

	// ����˳��ź�
	virtual BOOL TryWaitQuit(uint32 uMilliseconds = 0);

	//  �߳��Ƿ��˳�
	virtual BOOL IsExitThread();

	// ����ֹͣ����
	virtual void PostStop();

	// �ȴ��߳̽���,������Դ
	virtual void Join();

	// ɱ���߳�(������ʹ��,��ʹ��PostStop��֪ͨ�߳̽���)
	void Kill();

	// ��ȡ�߳�ID
	uint32 ThreadID() { return m_uThreadID; };


public:
	// ��ȡ���߳�ID
	static uint32 SelfID();

	// �߳�����ָ������ֵ
	static void Sleep(uint32 nMillisecond);

#ifdef OS_WINDOWS
	DWORD	 GetExitCode(){  DWORD dwExitCode; GetExitCodeThread (m_hThread, &dwExitCode); return dwExitCode;};
#endif//OS_WINDOWS

protected:
	virtual void Entry();

	// �߳����ѽ���(������ʹ��,��ֱ��return�˳��߳�ִ�к���)
	void Exit();

private:
#ifdef OS_WINDOWS
	static unsigned UPVAPI _Entry(void* pParam);
	HANDLE				m_hThread;
#endif//OS_WINDOWS
#ifdef OS_LINUX
	static void* UPVAPI _Entry(void *pParam);
#endif//OS_LINUX

private:
	uint32				m_uThreadID;
	PFTHREADPROC		m_pfThreadProc;
	void				*m_pParam;

	UVPEvent			m_evQuit;		// �������ʹ���ֹ������¼�
};

} // namespace uvp

using namespace uvp;

#endif//_UVP_THREAD_H_
