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
	afx_msg void OnLButtonUp(UINT nFlags, CPoint pt);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	DECLARE_MESSAGE_MAP();
private:
	WinBoard* m_pwb;
	Game m_game;
	WinPieces m_wp;

	bool m_piece_caught;
	Pos m_caught_pos;
	CPoint m_caught_pt;
};

CMainFrame::CMainFrame() :m_pwb(nullptr), m_game(), m_wp(&m_game, m_pwb), m_piece_caught(false)
{
	Create(NULL, L"Draughts", WS_OVERLAPPEDWINDOW, CRect(0, 0, 800, 800));
	CClientDC dc(this);
	CRect cr;
	GetClientRect(cr);
	m_pwb = new WinBoard(&cr, &dc);
	m_wp.Update();
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
	if (!m_game.IsSideSet())
	{
		if (m_pwb->InsideWhiteButton(pt))
			m_game.set_AIside(BLACK);
		else if (m_pwb->InsideBlackButton(pt))
			m_game.set_AIside(WHITE);
		Invalidate();
		return;
	}
	if (m_pwb->InsideResetButton(pt))
	{
		m_game.Reset();
		Invalidate();
		return;
	}
	m_caught_pos = m_pwb->SquareOfPt(pt);
	Piece* pp = (m_game.get_Board())(m_caught_pos.letter, m_caught_pos.digit);
	if (pp != nullptr && pp->White() == !m_game.AIside())
	{
		m_caught_pt = pt;
		m_piece_caught = true;
	}
}
inline void CMainFrame::OnLButtonUp(UINT nFlags, CPoint pt)
{
	if (m_piece_caught)
	{
		Game::Dirn dirn;
			if (pt.x > m_caught_pt.x)
				dirn = (pt.y > m_caught_pt.y) ? Game::Dirn::rightup : Game::Dirn::rightdown;
			else
				dirn = (pt.y > m_caught_pt.y) ? Game::Dirn::leftup : Game::Dirn::leftdown;
		bool moved = m_game.PlayerTurn(m_caught_pos.letter, m_caught_pos.digit, dirn);
		m_piece_caught = false;
		if (moved)
			m_game.AITurn();
		Invalidate();
	}
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
	ON_WM_LBUTTONUP()
	ON_WM_SIZE()
END_MESSAGE_MAP()