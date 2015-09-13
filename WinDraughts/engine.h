#pragma once
#include<stdexcept>
#include<string>
#include<utility>
#include"basic_classes.h"
#include<list>

#ifdef _CONSOLE
 #include<iostream>
#else
 #include"chessboard.h"
#endif

#ifndef FSIZE
#define FSIZE 8
#endif

#define WIN 1
#define DRAW 0
#define LOSS -1
#define MAX_RESULT WIN
#define MIN_RESULT LOSS
#include"AlphaBeta.h"

namespace Draughts
{
	typedef Field<FSIZE> Board;
#ifndef _CONSOLE
	typedef ChessBoard<FSIZE> DraughtsBoard;
#endif
	typedef char Result;
	typedef INodeTool<IField, Result> DraughtsTool;
	typedef MoveFinder MFinder;
	typedef RightJump RJump;
	typedef LeftJump LJump;
	typedef RightMove RMove;
	typedef LeftMove LMove;
	typedef std::set<Piece*> pset;
	//-------------------------------------------------------------------------------------------------
	class IStateFinder
	{
	public:
		IStateFinder(IField* pnode, std::set<IField*>& resvec, color turn) 
			:m_turn(turn), m_pboard(pnode), m_pvec(&resvec), m_mf(pnode)
		{
			for (char letter = 'a'; letter < 'a' + m_pboard->Size(); ++letter)
				for (unsigned int num = 1; num <= m_pboard->Size(); ++num)
				{
					if (m_pboard->at(letter, num) != nullptr && m_pboard->at(letter, num)->White() == m_turn)
						m_pboard->at(letter, num)->Accept(m_mf);
				}
		}
		void set_ResultVec(std::set<IField*>& resvec)
		{
			m_pvec = &resvec;
		}
		virtual std::set<IField*>* FindStates() = 0;
		virtual bool Empty() const = 0;
		virtual void Clear() = 0;
	protected:
		color m_turn;
		IField* m_pboard;
		std::set<IField*>* m_pvec;
		MFinder m_mf;
	};
	class MoviesFinder :public IStateFinder
	{
	public:
		MoviesFinder(IField* pnode, std::set<IField*>& resvec, color turn) :IStateFinder(pnode, resvec, turn)
		{
			m_rmovies = m_mf.RMovies();
			m_lmovies = m_mf.LMovies();
		}
		bool Empty() const
		{
			return m_lmovies.empty() && m_rmovies.empty();
		}
		std::set<IField*>* FindStates()
		{
			for (std::set<Piece*>::const_iterator it = m_rmovies.begin(); it != m_rmovies.end(); ++it)
			{
				Iter_func<RMove>(*it, m_pboard);
			}
			for (std::set<Piece*>::const_iterator it = m_lmovies.begin(); it != m_lmovies.end(); ++it)
			{
				Iter_func<LMove>(*it, m_pboard);
			}
			return m_pvec;
		}
		void Clear()
		{
			m_rmovies.clear();
			m_lmovies.clear();
		}
	private:
		pset m_rmovies;
		pset m_lmovies;
		template <class Visitor> void Iter_func(Piece* piece, IField* pb)
		{
			if (piece->King()) 
			{
				DirectionOf(piece) = UP;
				OneDirn_move<Visitor>(piece, pb);
				DirectionOf(piece) = DOWN;
			}
			OneDirn_move<Visitor>(piece, pb);
		}
		template <class Visitor> void OneDirn_move(Piece* piece, IField* pb)
		{
			IField* pnewboard = new Board(pb);
			Piece *p = pnewboard->at(pb->Lpos_of(piece), pb->Npos_of(piece));
			p->Accept(Visitor(pnewboard));
			m_pvec->insert(pnewboard);
		}
	};
	class JumpiesFinder :public IStateFinder
	{
	public:
		JumpiesFinder(IField* pnode, std::set<IField*>& resvec, color turn) :IStateFinder(pnode, resvec, turn)
		{
			m_rjumpies = m_mf.RJumpies();
			m_ljumpies = m_mf.LJumpies();
		}
		bool Empty() const
		{
			return m_ljumpies.empty() && m_rjumpies.empty();
		}
		std::set<IField*>* FindStates()
		{
			for (std::set<Piece*>::const_iterator it = m_rjumpies.begin(); it != m_rjumpies.end(); ++it)
			{
				Rec_func<RJump>(*it, m_pboard);
			}
			for (std::set<Piece*>::const_iterator it = m_ljumpies.begin(); it != m_ljumpies.end(); ++it)
			{
				Rec_func<LJump>(*it, m_pboard);
			}
			return m_pvec;
		}
		void Clear()
		{
			m_rjumpies.clear();
			m_ljumpies.clear();
		}
	private:
		pset m_rjumpies;
		pset m_ljumpies;
		template <class Visitor> void Rec_func(Piece* piece, IField* pb)
		{
			if (piece->King())
			{
				DirectionOf(piece) = UP;
				OneDirn_jump<Visitor>(piece, pb);
				DirectionOf(piece) = DOWN;
			}
			OneDirn_jump<Visitor>(piece, pb);
		}
		template <class Visitor> void OneDirn_jump(Piece* piece, IField* pb)
		{
			IField* pnewboard = new Board(pb);
			Piece *p = pnewboard->at(pb->Lpos_of(piece), pb->Npos_of(piece));
			p->Accept(Visitor(pnewboard)); // initial jump

			MFinder newmf(pnewboard);
			p->Accept(newmf);
			if (newmf.RJumpies().empty() && newmf.LJumpies().empty())
			{	// no multiple jumps
				m_pvec->insert(pnewboard); // ERROR: never executed
			}
			else
			{	// multiple jumps
				if (!newmf.RJumpies().empty())
					Rec_func<RJump>(p, pnewboard);
				if (!newmf.LJumpies().empty())
					Rec_func<LJump>(p, pnewboard);
				delete pnewboard;
			}
		}
	};
	//-------------------------------------------------------------------------------------------------
	class BoardTool :public DraughtsTool
	{
	public:
		explicit BoardTool(color& AIside) :m_pcf(nullptr), m_pnode(nullptr), m_AIside(AIside)
		{ }
		void set_Node(IField* board)
		{
			m_pnode = board;
		}
		Result NodeValue() const
		{
			bool winning_side;
			if (m_pnode->Win(winning_side))
				return ((winning_side == m_AIside) ? WIN : LOSS);
			return DRAW;
		}
		bool TerminalNode() const
		{
			bool dummy = true;
			return m_pnode->Win(dummy) || m_pnode->Draw();
		}
		std::set<IField*> ChildNodes(bool maximizing_side)
		{
			bool turn = (maximizing_side) ? m_AIside : (!m_AIside);			
			std::set<IField*> temp;

			m_pcf = new JumpiesFinder(m_pnode, temp, turn);
			if (!(m_pcf->Empty())) // jump
			{
				m_pcf->FindStates();
			}
			else // move
			{
				delete m_pcf;
				m_pcf = new MoviesFinder(m_pnode, temp, turn);
				m_pcf->FindStates();
			}
			delete m_pcf;
			m_pcf = nullptr;
			return temp;
		}
	private:
		IStateFinder* m_pcf;
		IField* m_pnode;
		color& m_AIside;
	};
	//-------------------------------------------------------------------------------------------------
	class Game
	{
	public:
		Game() :m_pboard(new Board), m_pbt(nullptr), m_pAB(nullptr), m_side_set(false)
		{ }
		~Game()
		{
			if (m_pbt != nullptr) delete m_pbt;
			if (m_pAB != nullptr) delete m_pAB;
		}
		void set_AIside(bool AIside)
		{
			m_side_set = true;
			m_AIside = AIside;
			m_pbt = new BoardTool(m_AIside);
			m_pAB = new AlphaBeta<BoardTool>(m_pbt, false);
		}
		void Reset()
		{
			if (m_pAB != nullptr)
			{
				delete m_pAB;
				m_pAB = nullptr;
			}
			if (m_pbt != nullptr)
			{
				delete m_pbt;
				m_pbt = nullptr;
			}
			m_side_set = false;
			m_pboard->Reset();
		}
		bool IsSideSet() const
		{
			return m_side_set;
		}
		color AIside() const
		{
			if (!m_side_set)
				throw std::logic_error("AI side is not set");
			return m_AIside;
		}
		const IField* get_Board() const
		{
			return m_pboard;
		}
		bool AITurn()
		{
			if (!m_side_set)
				return false;
			IField* pnewstate = m_pAB->NextState(m_pboard);
			delete m_pboard;
			m_pboard = pnewstate;
			return true;
		}
		enum class Dirn
		{
			rightup, rightdown, leftup, leftdown
		};
		bool PlayerTurn(char letter, unsigned int num, Game::Dirn direction)
		{
			if (!m_side_set)
				return false;
			IOperation* pop;
			Piece* p = m_pboard->at(letter, num);
			MFinder mf(m_pboard);
			p->Accept(mf);
			if (direction == Dirn::rightup || direction == Dirn::rightdown)
			{
				pset rjumpies = mf.RJumpies();
				pset rmovies = mf.RMovies();
				if (rjumpies.find(p) != rjumpies.end())
					pop = new RJump(m_pboard);
				else if (rmovies.find(p) != rmovies.end())
					pop = new RMove(m_pboard);
				else
					return false;
			}
			else // leftup or leftdown
			{
				pset ljumpies = mf.LJumpies();
				pset lmovies = mf.LMovies();
				if (ljumpies.find(p) != ljumpies.end())
					pop = new LJump(m_pboard);
				else if (lmovies.find(p) != lmovies.end())
					pop = new LMove(m_pboard);
				else
					return false;
			}
			if (p->King())
				DirectionOf(p) = (direction == Dirn::leftup) ? UP : DOWN;
			p->Accept(*pop);
			delete pop;
			return true;
		}
	private:
		IField* m_pboard;
		BoardTool* m_pbt;
		AlphaBeta<BoardTool>* m_pAB;
		bool m_AIside;
		bool m_side_set;
	};
	//-------------------------------------------------------------------------------------------------
#ifdef _CONSOLE
	class CmdPlay
	{
	public:
		CmdPlay() :m_lj(&m_f), m_rj(&m_f), m_lm(&m_f), m_rm(&m_f), m_mf(&m_f)
		{ }
		void Run() // no AI
		{
			char letter, dirn;
			unsigned int num;

			while (1)
			{
				std::cout << m_f;
				std::cout << "Enter letter, number, direction(l/r): ";
				std::cin >> letter >> num >> dirn;

				if (m_f.at(letter, num) != nullptr /*&& m_f(letter, num)->White()*/)
				{
					if (m_f.at(letter, num)->King())
					{
						char vdirn;
						std::cout << "Enter king's vertical direction (u/d): ";
						std::cin >> vdirn;
						DirectionOf(m_f.at(letter, num)) = ((vdirn == 'u') ? UP : DOWN);
					}
					m_f.at(letter, num)->Accept(m_mf);
					if (dirn == 'r')
						(m_mf.RJumpies().empty()) ? m_f.at(letter, num)->Accept(m_rm) : m_f.at(letter, num)->Accept(m_rj);
					else if (dirn == 'l')
						(m_mf.LJumpies().empty()) ? m_f.at(letter, num)->Accept(m_lm) : m_f.at(letter, num)->Accept(m_lj);
					else
						break;
				}
				else
					break;
			}
		}
	private:
		// Board:
		Field<FSIZE> m_f;
		// Visitors:
		LeftJump m_lj;
		RightJump m_rj;
		LeftMove m_lm;
		RightMove m_rm;
		MoveFinder m_mf;
	};
#else
#pragma warning( push )
#pragma warning( disable : 4800 )
#pragma warning( disable : 4244 )
	class WinBoard :public DraughtsBoard
	{
	public:
		WinBoard(CRect* pclient_rect, CDC* pDC) :DraughtsBoard(pclient_rect, pDC)
		{
			CSize color_button_size = pDC->GetTextExtent("White");
			color_button_size.cx += 10;
			color_button_size.cy += 10;
			CSize reset_size = pDC->GetTextExtent("New game");
			reset_size.cx += 10;
			reset_size.cy += 10;
			CRect edge = ButtonsField();

			m_white_button = CRect(edge.TopLeft(), color_button_size);
			m_black_button = CRect(edge.TopLeft(), color_button_size);
			m_black_button.OffsetRect(m_white_button.Size().cx * 1.5, 0);
			m_reset_button.right = edge.right;
			m_reset_button.top = edge.top;
			m_reset_button.bottom = m_reset_button.top + reset_size.cy;
			m_reset_button.left = m_reset_button.right - reset_size.cx;
		}
		bool InsideWhiteButton(const CPoint& pt) const
		{
			return (bool)(m_white_button.PtInRect(pt));
		}
		bool InsideBlackButton(const CPoint& pt) const
		{
			return (bool)(m_black_button.PtInRect(pt));
		}
		bool InsideResetButton(const CPoint& pt) const
		{
			return (bool)(m_reset_button.PtInRect(pt));
		}
		void Display(CDC* pDC, Game* pgame)
		{
			DraughtsBoard::Display(pDC);
			pDC->SelectStockObject(HOLLOW_BRUSH);
			pDC->Rectangle(m_reset_button);
			if (!pgame->IsSideSet())
			{
				pDC->SelectStockObject(HOLLOW_BRUSH);
				pDC->Rectangle(m_white_button);
				pDC->SelectStockObject(HOLLOW_BRUSH);
				pDC->Rectangle(m_black_button);
			}
			else
			{
				(pgame->AIside() == WHITE) ? pDC->SelectStockObject(HOLLOW_BRUSH) : pDC->SelectStockObject(BLACK_BRUSH);
				pDC->Rectangle(m_white_button);
				(pgame->AIside() == WHITE) ? pDC->SelectStockObject(BLACK_BRUSH) : pDC->SelectStockObject(HOLLOW_BRUSH);
				pDC->Rectangle(m_black_button);
			}
			pDC->DrawText("White", m_white_button, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
			pDC->DrawText("Black", m_black_button, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
			pDC->DrawText("New game", m_reset_button, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
		}
	private:
		CRect m_white_button;
		CRect m_black_button;
		CRect m_reset_button;
	};
#pragma warning( pop )
	class WinPieces
	{
	public:
		WinPieces(Game* game, WinBoard*& pwb) :m_pgame(game), m_ppwb(&pwb)
		{
			if ((*m_ppwb) != nullptr)
				Update();
		}
		void Update()
		{
			m_whitemen.clear();
			m_blackmen.clear();
			m_whitekings.clear();
			m_blackkings.clear();
			for (unsigned int row = 1; row <= FSIZE; ++row)
				for (char letter = 'a'; letter < 'a' + FSIZE; ++letter)
				{
					Piece* pp = (m_pgame->get_Board())->at(letter, row);
					if (pp != nullptr)
					{
						CRect temp = (*m_ppwb)->SquareAt(letter, row);
						int margin = (*m_ppwb)->SquareSize() / 8;
						temp.right -= (2 * margin);
						temp.bottom -= (2 * margin);
						temp.OffsetRect(margin, margin);
						if (!pp->King())
							pp->White() ? m_whitemen.push_back(temp) : m_blackmen.push_back(temp);
						else
							pp->White() ? m_whitekings.push_back(temp) : m_blackkings.push_back(temp);
					}
				}
		}
		void Display(CDC* pDC)
		{
			for (std::list<CRect>::const_iterator it = m_whitemen.begin(); it != m_whitemen.end(); ++it)
			{
				pDC->SelectStockObject(WHITE_BRUSH);
				pDC->Ellipse(*it);
			}
			for (std::list<CRect>::const_iterator it = m_blackmen.begin(); it != m_blackmen.end(); ++it)
			{
				pDC->SelectStockObject(BLACK_BRUSH);
				pDC->Ellipse(*it);
			}
			for (std::list<CRect>::const_iterator it = m_whitekings.begin(); it != m_whitekings.end(); ++it)
			{
				pDC->SelectStockObject(LTGRAY_BRUSH);
				pDC->Ellipse(*it);
			}
			for (std::list<CRect>::const_iterator it = m_blackkings.begin(); it != m_blackkings.end(); ++it)
			{
				pDC->SelectStockObject(DKGRAY_BRUSH);
				pDC->Ellipse(*it);
			}
		}
		CRect* get_Piece(const CPoint& pt)
		{
			for (std::list<CRect>::iterator it = m_whitemen.begin(); it != m_whitemen.end(); ++it)
				if (it->PtInRect(pt))
					return &(*it);
			for (std::list<CRect>::iterator it = m_blackmen.begin(); it != m_blackmen.end(); ++it)
				if (it->PtInRect(pt))
					return &(*it);
			for (std::list<CRect>::iterator it = m_whitekings.begin(); it != m_whitekings.end(); ++it)
				if (it->PtInRect(pt))
					return &(*it);
			for (std::list<CRect>::iterator it = m_blackkings.begin(); it != m_blackkings.end(); ++it)
				if (it->PtInRect(pt))
					return &(*it);
			return nullptr;
		}
	private:
		Game* m_pgame;
		WinBoard** m_ppwb;
		std::list<CRect> m_whitemen;
		std::list<CRect> m_blackmen;
		std::list<CRect> m_whitekings;
		std::list<CRect> m_blackkings;
	};
#endif
	//-------------------------------------------------------------------------------------------------
}