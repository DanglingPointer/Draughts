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
	WinBoard* m_pwb;
	Game m_game;
	WinPieces m_wp;
};

CMainFrame::CMainFrame() :m_pwb(nullptr), m_game(), m_wp(&m_game, m_pwb)
{
	Create(NULL, L"Draughts", WS_OVERLAPPEDWINDOW, CRect(0, 0, 800, 800));
	CClientDC dc(this);
	CRect cr;
	GetClientRect(cr);
	m_pwb = new WinBoard(&cr, &dc);
	CenterWindow();
}
inline CMainFrame::~CMainFrame()
{
	if (m_pwb != nullptr)
		delete m_pwb;
}
inline void CMainFrame::OnPaint()
{
	m_wp.Update();
	CPaintDC dc(this);
	m_pwb->Display(&dc, &m_game);
	m_wp.Display(&dc);
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
	m_pwb = new WinBoard(&cr, &dc);
}

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_SIZE()
END_MESSAGE_MAP()