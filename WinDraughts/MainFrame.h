#pragma once
#include<afxwin.h>
#include"Tictactoe.h"

#ifdef ALPHABETA
#define ALG ttt::AlphaBeta
#else
#define ALG ttt::Minimax
#endif

using namespace ttt;

class CMainFrame :public CFrameWnd
{
public:
	CMainFrame();
	void AIMakeMove();
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint pt);
	DECLARE_MESSAGE_MAP();
private:
	Mark m_winning_side;
	CRect m_wr, m_crosses_button, m_noughts_button, m_reset_button;
	int m_wr_cx, m_wr_cy;
	Gameplay m_ttt;
};

CMainFrame::CMainFrame() :m_winning_side(Mark::empty)
{
	Create(NULL, L"Tic-tac-toe", WS_OVERLAPPEDWINDOW & ~WS_MAXIMIZEBOX & ~WS_SIZEBOX, CRect(0, 0, 500, 530));

	GetClientRect(m_wr);
	m_wr.top += 50;
	m_wr.left += 50;
	m_wr.bottom -= 80;
	m_wr.right -= 50;
	m_wr.OffsetRect(0, 40);
	m_wr_cx = m_wr.Size().cx;
	m_wr_cy = m_wr.Size().cy;

	CClientDC dc(this);
	m_crosses_button = CRect(m_wr.left, 30, m_wr.left + dc.GetTextExtent("Crosses").cx + 10, 30 + dc.GetTextExtent("Crosses").cy + 10);

	int left = m_wr.left + dc.GetTextExtent("Crosses").cx + 10;
	m_noughts_button = CRect(left + 30, 30, left + 30 + dc.GetTextExtent("Crosses").cx + 10, 30 + dc.GetTextExtent("Noughts").cy + 10);

	m_reset_button = CRect(m_wr.right - dc.GetTextExtent("New game").cx - 10, 30, m_wr.right, 30 + dc.GetTextExtent("New game").cy + 10);

	CenterWindow();
}

inline void CMainFrame::OnPaint()
{
	CPaintDC dc(this);
	// Grid border
	dc.Rectangle(m_wr);
	// Vertical grid
	CRect vrect(m_wr.left + m_wr_cx / 3, m_wr.top, m_wr.right - m_wr_cx / 3, m_wr.bottom);
	dc.Rectangle(vrect);
	// Horisontal grid
	dc.SelectStockObject(HOLLOW_BRUSH);
	CRect hrect(m_wr.left, m_wr.top + m_wr_cy / 3, m_wr.right, m_wr.bottom - m_wr_cy / 3);
	dc.Rectangle(hrect);
	// Nought button
	(m_ttt.side == Mark::nought) ? dc.SelectStockObject(BLACK_BRUSH) : dc.SelectStockObject(HOLLOW_BRUSH);
	dc.Rectangle(m_crosses_button);
	dc.DrawTextW("Crosses", m_crosses_button, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
	// Cross button
	(m_ttt.side == Mark::cross) ? dc.SelectStockObject(BLACK_BRUSH) : dc.SelectStockObject(HOLLOW_BRUSH);
	dc.Rectangle(m_noughts_button);
	dc.DrawTextW("Noughts", m_noughts_button, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
	// Reset button
	dc.SelectStockObject(HOLLOW_BRUSH);
	dc.Rectangle(m_reset_button);
	dc.DrawTextW("New game", m_reset_button, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
	// Marks
	for (uint row = 0; row < 3; ++row)
		for (uint col = 0; col < 3; ++col)
			if (m_ttt.pgrid->at(row, col) == Mark::cross)
			{
				int left = m_wr.left + m_wr_cx / 6 - (dc.GetTextExtent("X").cx) / 2 + col * m_wr_cx / 3;
				int right = left + dc.GetTextExtent("X").cx;
				int top = m_wr.top + m_wr_cy / 6 - (dc.GetTextExtent("X").cy) / 2 + row * m_wr_cy / 3;
				int bottom = top + dc.GetTextExtent("X").cy;
				dc.DrawTextW("X", CRect(left, top, right, bottom), DT_SINGLELINE | DT_CENTER | DT_VCENTER);
			}
			else if (m_ttt.pgrid->at(row, col) == Mark::nought)
			{
				int left = m_wr.left + m_wr_cx / 6 - (dc.GetTextExtent("O").cx) / 2 + col * m_wr_cx / 3;
				int right = left + dc.GetTextExtent("O").cx;
				int top = m_wr.top + m_wr_cy / 6 - (dc.GetTextExtent("O").cy) / 2 + row * m_wr_cy / 3;
				int bottom = top + dc.GetTextExtent("O").cy;
				dc.DrawTextW("O", CRect(left, top, right, bottom), DT_SINGLELINE | DT_CENTER | DT_VCENTER);
			}
	// Result message
	if (m_winning_side == Mark::empty)
		m_winning_side = m_ttt.pgrid->Won();
	if (m_winning_side != Mark::empty)
	{
		if (m_winning_side == m_ttt.side)
			MessageBoxW(L"You lost!", L"Condolences!");
		else
			MessageBoxW(L"You won!", L"Congratulations!");
	}
}

inline void CMainFrame::OnLButtonDown(UINT nFlags, CPoint pt)
{
	// Choosing side
	if (m_ttt.side == Mark::empty && m_crosses_button.PtInRect(pt))
	{
		m_ttt.side = Mark::nought;
		Invalidate(TRUE);
		return;
	}
	if (m_ttt.side == Mark::empty && m_noughts_button.PtInRect(pt))
	{
		m_ttt.side = Mark::cross;
		// First move (random)
		uint row = rand() % 3;
		uint col = rand() % 3;
		m_ttt.pgrid->SetCross(row, col);

		Invalidate(TRUE);
		return;
	}
	// New game
	if (m_reset_button.PtInRect(pt))
	{
		m_winning_side = Mark::empty;
		m_ttt.Reset();
		Invalidate(TRUE);
		return;
	}
	// Changing state
	int col = -1;
	if (pt.x > m_wr.left)
	{
		if (pt.x < m_wr.left + m_wr_cx / 3)	col = 0;
		else if (pt.x < m_wr.right - m_wr_cx / 3) col = 1;
		else if (pt.x < m_wr.right) col = 2;
	}
	int row = -1;
	if (pt.y > m_wr.top)
	{
		if (pt.y < m_wr.top + m_wr_cy / 3) row = 0;
		else if (pt.y < m_wr.bottom - m_wr_cy / 3) row = 1;
		else if (pt.y < m_wr.bottom) row = 2;
	}
	if (!(row == -1 || col == -1) && m_ttt.pgrid->at(row, col) == Mark::empty)
	{
		if (m_ttt.side == Mark::cross)
		{
			m_ttt.pgrid->SetNought(row, col);
			AIMakeMove(); // without command prompt
		}
		else if (m_ttt.side == Mark::nought)
		{
			m_ttt.pgrid->SetCross(row, col);
			AIMakeMove(); // without command prompt
		}
		else
			MessageBoxW(L"Choose side first!", L"Error");
	}
}

inline void CMainFrame::AIMakeMove()
{
	m_winning_side = m_ttt.pgrid->Won();
	if (m_winning_side == Mark::empty)
		m_ttt.AITurn<ALG>();
	Invalidate(TRUE);
}

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()