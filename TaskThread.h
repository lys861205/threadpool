#include "UVPThread.h"
#include "UVPEvent.h"
//---------------------------------------------------------------------------
template<class T>
class CTaskThread : public UVPThread
{
public:
	CTaskThread(int number, T *processer = NULL) : m_Processer(processer), m_nThreadNum(number),m_evProcess(true)
	{
		
	}


	~CTaskThread()
	{

	}


    virtual void Entry()
	{
		if (!m_Processer)
			return;
		while (m_bRunning)
		{
			m_Processer->Worker(this);
		}
	}

	void Start()
	{
		m_bRunning = TRUE;
		m_evProcess.Reset();
		UVPThread::Start();
	}

	void Stop()
	{
		m_bRunning = FALSE;
		m_evProcess.Set();

		for (int i = 0; i < 1000; i++)
		{
			if ( m_hThread == NULL )
			{
				break;
			}

			Sleep(1);
		}			
	
		if (m_hThread == NULL)
		{
			Kill();
		}
	}

	UVPEvent m_evProcess;
	int  m_nThreadNum;
private:
	bool m_bRunning;
	T *  m_Processer;
};