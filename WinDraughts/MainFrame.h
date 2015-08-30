#pragma once
#include<afxwin.h>
#include"engine.h"
using namespace Draughts;

class CMainFrame :public CFrameWnd
{
public:
	CMainFrame();
	~CMainFrame();
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint pt);
	DECLARE_MESSAGE_MAP();
private:
	WinBoard<FSIZE>* m_pwb;
};

CMainFrame::CMainFrame() :m_pwb(nullptr)
{
	Create(NULL, L"Draughts", WS_OVERLAPPEDWINDOW & ~WS_MAXIMIZEBOX & ~WS_SIZEBOX, CRect(0, 0, 1000, 1000));
	CClientDC dc(this);
	CRect cr;
	GetClientRect(cr);
	m_pwb = new WinBoard<FSIZE>(&cr, &dc);
	CenterWindow();
}
inline CMainFrame::~CMainFrame()
{
	if (m_pwb != nullptr)
		delete m_pwb;
}
inline void CMainFrame::OnPaint()
{
	CPaintDC dc(this);
	m_pwb->Display(&dc);
}
inline void CMainFrame::OnLButtonDown(UINT nFlags, CPoint pt)
{
	//
}
BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()