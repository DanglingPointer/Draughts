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
	afx_msg void OnSize(UINT nType, int cx, int cy);
	DECLARE_MESSAGE_MAP();
private:
	WinBoard<FSIZE>* m_pwb;
};

CMainFrame::CMainFrame() :m_pwb(nullptr)
{
	Create(NULL, L"Draughts", WS_OVERLAPPEDWINDOW, CRect(0, 0, 800, 800));
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
inline void CMainFrame::OnSize(UINT nType, int cx, int cy)
{
	if (m_pwb != nullptr) delete m_pwb;
	CClientDC dc(this);
	CRect cr;
	GetClientRect(cr);
	m_pwb = new WinBoard<FSIZE>(&cr, &dc);
}

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_SIZE()
END_MESSAGE_MAP()