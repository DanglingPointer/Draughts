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
	typedef char Result;
	typedef INodeTool<Board, Result> DraughtsTool;
	typedef MoveFinder<FSIZE> MFinder;
	typedef RightJump<FSIZE> RJump;
	typedef LeftJump<FSIZE> LJump;
	typedef RightMove<FSIZE> RMove;
	typedef LeftMove<FSIZE> LMove;
	typedef std::set<Piece*> pset;
	//-------------------------------------------------------------------------------------------------
	class IStateFinder
	{
	public:
		IStateFinder(Board* pnode, std::set<Board*>& resvec, color turn) 
			:m_turn(turn), m_pboard(pnode), m_pvec(&resvec), m_mf(*pnode), m_pop(nullptr)
		{
			Board& board = *m_pboard;
			for (char letter = 'a'; letter < 'a' + FSIZE; ++letter)
				for (unsigned int num = 1; num <= FSIZE; ++num)
				{
					if (board(letter, num) != nullptr && board(letter, num)->White() == m_turn)
						board(letter, num)->Accept(m_mf);
				}
		}
		void set_ResultVec(std::set<Board*>& resvec)
		{
			m_pvec = &resvec;
		}
		void set_Board(Board* pb)
		{
			m_pboard = pb;
		}
		virtual std::set<Board*>* FindStates() = 0;
		virtual bool Empty() const = 0;
		virtual void Clear() = 0;
	protected:
		color m_turn;
		Board* m_pboard;
		std::set<Board*>* m_pvec;
		MFinder m_mf;
		Operation* m_pop;
		virtual void Rec_func(Piece* piece, Board* pb) = 0;
		template <class Visitor> void Loop_func(Piece* p, Board* pb)
		{
			m_pop = new Visitor(*pb);
			if (p->King())
			{
				DirectionOf(p) = UP;
				Rec_func(p, pb);
				delete m_pop;
				m_pop = new Visitor(*pb);
				DirectionOf(p) = DOWN;
			}
			Rec_func(p, pb);
			delete m_pop;
		}
	};
	class MoviesFinder :public IStateFinder
	{
	public:
		MoviesFinder(Board* pnode, std::set<Board*>& resvec, color turn) :IStateFinder(pnode, resvec, turn)
		{
			m_rmovies = m_mf.RMovies();
			m_lmovies = m_mf.LMovies();
		}
		bool Empty() const
		{
			return m_lmovies.empty() && m_rmovies.empty();
		}
		std::set<Board*>* FindStates()
		{
			for (std::set<Piece*>::const_iterator it = m_rmovies.begin(); it != m_rmovies.end(); ++it)
			{
				Loop_func<RMove>(*it, m_pboard);
			}
			for (std::set<Piece*>::const_iterator it = m_lmovies.begin(); it != m_lmovies.end(); ++it)
			{
				Loop_func<LMove>(*it, m_pboard);
			}
			m_pop = nullptr;
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
		void Rec_func(Piece* piece, Board* pb)
		{
			Board* pnewboard = new Board(*pb);
			Piece *p = pnewboard->operator()(m_pboard->Lpos_of(piece), m_pboard->Npos_of(piece));
			p->Accept(*m_pop);
			m_pvec->insert(pnewboard);
		}
	};
	class JumpiesFinder :public IStateFinder
	{
	public:
		JumpiesFinder(Board* pnode, std::set<Board*>& resvec, color turn) :IStateFinder(pnode, resvec, turn)
		{
			m_rjumpies = m_mf.RJumpies();
			m_ljumpies = m_mf.LJumpies();
		}
		bool Empty() const
		{
			return m_ljumpies.empty() && m_rjumpies.empty();
		}
		std::set<Board*>* FindStates()
		{
			for (std::set<Piece*>::const_iterator it = m_rjumpies.begin(); it != m_rjumpies.end(); ++it)
			{
				Loop_func<RJump>(*it, m_pboard);
			}
			for (std::set<Piece*>::const_iterator it = m_ljumpies.begin(); it != m_ljumpies.end(); ++it)
			{
				Loop_func<LJump>(*it, m_pboard);
			}
			m_pop = nullptr;
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
		void Rec_func(Piece* piece, Board* pb)
		{
			Board* pnewboard = new Board(*pb);
			Piece *p = pnewboard->operator()(m_pboard->Lpos_of(piece), m_pboard->Npos_of(piece));
			p->Accept(*m_pop); // initial jump

			MFinder newmf(*pnewboard);
			p->Accept(newmf);
			if (newmf.RJumpies().empty() && newmf.LJumpies().empty())
			{	// no multiple jumps
				m_pvec->insert(pnewboard);
			}
			else
			{	// multiple jumps
				if (!newmf.RJumpies().empty())
					Loop_func<RJump>(p, pnewboard);
				if (!newmf.LJumpies().empty())
					Loop_func<LJump>(p, pnewboard);
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
		void set_Node(Board* board)
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
		std::set<Board*> ChildNodes(bool maximizing_side)
		{
			bool turn = (maximizing_side) ? m_AIside : (!m_AIside);			
			std::set<Board*> temp;

			m_pcf = new JumpiesFinder(m_pnode, temp, turn);
			if (!m_pcf->Empty()) // jump
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
		Board* m_pnode;
		color& m_AIside;
	};
	//-------------------------------------------------------------------------------------------------
	class Game
	{
	public:
		Game() :m_pbt(nullptr), m_pAB(nullptr), m_side_set(false)
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
			m_pAB = new AlphaBeta<BoardTool>(m_pbt);
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
			m_board.Reset();
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
		const Board& get_Board() const
		{
			return m_board;
		}
		bool AITurn()
		{
			if (!m_side_set)
				return false;
			Board* pnewstate = m_pAB->NextState(&m_board);
			m_board = *pnewstate;
			delete pnewstate;
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
			Operation* pop;
			Piece* p = m_board(letter, num);
			MFinder mf(m_board);
			p->Accept(mf);
			if (direction == Dirn::rightup || direction == Dirn::rightdown)
			{
				pset rjumpies = mf.RJumpies();
				pset rmovies = mf.RMovies();
				if (rjumpies.find(p) != rjumpies.end())
					pop = new RJump(m_board);
				else if (rmovies.find(p) != rmovies.end())
					pop = new RMove(m_board);
				else
					return false;

				if (p->King())
					DirectionOf(p) = (direction == Dirn::rightup) ? UP : DOWN;
				p->Accept(*pop);
				delete pop;
				return true;
			}
			else // leftup or leftdown
			{
				pset ljumpies = mf.LJumpies();
				pset lmovies = mf.LMovies();
				if (ljumpies.find(p) != ljumpies.end())
					pop = new LJump(m_board);
				else if (lmovies.find(p) != lmovies.end())
					pop = new LMove(m_board);
				else
					return false;

				if (p->King())
					DirectionOf(p) = (direction == Dirn::leftup) ? UP : DOWN;
				p->Accept(*pop);
				delete pop;
				return true;
			}
		}
	private:
		Board m_board;
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
		CmdPlay() :m_lj(m_f), m_rj(m_f), m_lm(m_f), m_rm(m_f), m_mf(m_f)
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

				if (m_f(letter, num) != nullptr && m_f(letter, num)->White())
				{
					if (m_f(letter, num)->King())
					{
						char vdirn;
						std::cout << "Enter king's vertical direction (u/d): ";
						std::cin >> vdirn;
						Direction* pd = &DirectionOf(m_f(letter, num));
						*pd  = ((vdirn == 'u') ? UP : DOWN);
					}
					m_f(letter, num)->Accept(m_mf);
					if (dirn == 'r')
						(m_mf.RJumpies().empty()) ? m_f(letter, num)->Accept(m_rm) : m_f(letter, num)->Accept(m_rj);
					else if (dirn == 'l')
						(m_mf.LJumpies().empty()) ? m_f(letter, num)->Accept(m_lm) : m_f(letter, num)->Accept(m_lj);
					else
						break;
				}
			}
		}
	private:
		// Board:
		Field<FSIZE> m_f;
		// Visitors:
		LeftJump<FSIZE> m_lj;
		RightJump<FSIZE> m_rj;
		LeftMove<FSIZE> m_lm;
		RightMove<FSIZE> m_rm;
		MoveFinder<FSIZE> m_mf;
	};
#else
#pragma warning( push )
#pragma warning( disable : 4800 )
#pragma warning( disable : 4244 )
	class WinBoard :public ChessBoard<FSIZE>
	{
	public:
		WinBoard(CRect* pclient_rect, CDC* pDC) :ChessBoard<FSIZE>(pclient_rect, pDC)
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
			ChessBoard<FSIZE>::Display(pDC);
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
		{ }
		void Update()
		{
			m_whitemen.clear();
			m_blackmen.clear();
			m_whitekings.clear();
			m_blackkings.clear();
			for (unsigned int row = 1; row <= FSIZE; ++row)
				for (char letter = 'a'; letter < 'a' + FSIZE; ++letter)
				{
					Piece* pp = (m_pgame->get_Board())(letter, row);
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