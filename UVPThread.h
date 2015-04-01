
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

	// 检测退出信号
	virtual BOOL TryWaitQuit(uint32 uMilliseconds = 0);

	//  线程是否退出
	virtual BOOL IsExitThread();

	// 发送停止命令
	virtual void PostStop();

	// 等待线程结束,回收资源
	virtual void Join();

	// 杀死线程(不建议使用,可使用PostStop来通知线程结束)
	void Kill();

	// 获取线程ID
	uint32 ThreadID() { return m_uThreadID; };


public:
	// 获取本线程ID
	static uint32 SelfID();

	// 线程休眠指定毫秒值
	static void Sleep(uint32 nMillisecond);

#ifdef OS_WINDOWS
	DWORD	 GetExitCode(){  DWORD dwExitCode; GetExitCodeThread (m_hThread, &dwExitCode); return dwExitCode;};
#endif//OS_WINDOWS

protected:
	virtual void Entry();

	// 线程自已结束(不建议使用,可直接return退出线程执行函数)
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

	UVPEvent			m_evQuit;		// 这里必须使用手工重设事件
};

} // namespace uvp

using namespace uvp;

#endif//_UVP_THREAD_H_
