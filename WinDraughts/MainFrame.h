#pragma once
#include<afxwin.h>

class CMainFrame :public CFrameWnd
{
public:
	CMainFrame();
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint pt);
	DECLARE_MESSAGE_MAP();
private:
	//
};

CMainFrame::CMainFrame() 
{
	Create(NULL, L"Tic-tac-toe", WS_OVERLAPPEDWINDOW & ~WS_MAXIMIZEBOX & ~WS_SIZEBOX, CRect(0, 0, 500, 530));

	CClientDC dc(this);

	CenterWindow();
}

inline void CMainFrame::OnPaint()
{
	CPaintDC dc(this);
}

inline void CMainFrame::OnLButtonDown(UINT nFlags, CPoint pt)
{
	//
}

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()