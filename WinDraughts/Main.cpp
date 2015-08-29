#include"MainFrame.h"

class CMyWinApp :public CWinApp
{
public:
	virtual BOOL InitInstance()
	{
		m_pMainWnd = new CMainFrame;
		m_pMainWnd->ShowWindow(SW_SHOW);
		m_pMainWnd->UpdateWindow();
		return TRUE;
	}
} MyApplication;