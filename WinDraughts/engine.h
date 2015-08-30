#pragma once
#include<stdexcept>
#include<string>
#include"basic_classes.h"
#ifndef _CONSOLE
#include<afxwin.h>
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

	class BoardTool :public DraughtsTool
	{
	public:
		explicit BoardTool(bool& sidecolor) : m_pnode(nullptr), m_AIside(sidecolor)
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
		std::set<Board*> ChildNodes(bool maximizing_side) const
		{
			bool turn = (maximizing_side) ? m_AIside : (!m_AIside);
			Board& board = *m_pnode;

			MFinder mf(board);
			for (char letter = 'a'; letter < 'a' + FSIZE; ++letter)
				for (unsigned int num = 1; num <= FSIZE; ++num)
				{
					if (board(letter, num) != nullptr && board(letter, num)->White() == turn)
						board(letter, num)->Accept(mf);
				}
			pset rjumpies = mf.RJumpies();
			pset ljumpies = mf.LJumpies();

			std::set<Board*> temp;
			Direction* pdirn = nullptr;
			if (!rjumpies.empty() || !ljumpies.empty()) // jump
			{
				Operation* pop = new RJump(board);
				for (std::set<Piece*>::const_iterator it = rjumpies.begin(); it != rjumpies.end(); ++it)
				{
					if ((*it)->King())
					{
						DirectionOf(*it) = UP;
						jump_iterator_func(temp, board, *it, pop);
						DirectionOf(*it) = DOWN;
					}
					jump_iterator_func(temp, board, *it, pop);
				}
				delete pop;
				pop = new LJump(board);
				for (std::set<Piece*>::const_iterator it = ljumpies.begin(); it != ljumpies.end(); ++it)
				{
					if ((*it)->King())
					{
						DirectionOf(*it) = UP;
						jump_iterator_func(temp, board, *it, pop);
						DirectionOf(*it) = DOWN;
					}
					jump_iterator_func(temp, board, *it, pop);
				}
				delete pop;
			}
			else // move
			{
				pset rmovies = mf.RMovies();
				pset lmovies = mf.LMovies();
				Operation* pop = new RMove(board);
				for (std::set<Piece*>::const_iterator it = rmovies.begin(); it != rmovies.end(); ++it)
				{
					if ((*it)->King())
					{
						DirectionOf(*it) = UP;
						move_iterator_func(temp, board, *it, pop);
						DirectionOf(*it) = DOWN;
					}
					move_iterator_func(temp, board, *it, pop);
				}
				delete pop;
				pop = new LMove(board);
				for (std::set<Piece*>::const_iterator it = lmovies.begin(); it != lmovies.end(); ++it)
				{
					if ((*it)->King())
					{
						DirectionOf(*it) = UP;
						move_iterator_func(temp, board, *it, pop);
						DirectionOf(*it) = DOWN;
					}
					move_iterator_func(temp, board, *it, pop);
				}
				delete pop;
			}
			return temp;
		}
	private:
		void move_iterator_func(std::set<Board*>& temp, const Board& board, Piece* piece, Operation* move) const
		{
			Board* pnewboard = new Board(board);
			Piece *p = pnewboard->operator()(board.Lpos_of(piece), board.Npos_of(piece));
			p->Accept(*move);
			temp.insert(pnewboard);
		}
		void jump_iterator_func(std::set<Board*>& temp, const Board& board, Piece* piece, Operation* jump) const
		{
			Board* pnewboard = new Board(board);
			Piece *p = pnewboard->operator()(board.Lpos_of(piece), board.Npos_of(piece));
			p->Accept(*jump);

			MFinder newmf(*pnewboard);
			pset newrjumpies = newmf.RJumpies();
			pset newljumpies = newmf.LJumpies();
			if (newrjumpies.find(p) == newrjumpies.end() && newljumpies.find(p) == newljumpies.end())
			{	// no multiple jumps
				temp.insert(pnewboard);
			}
			else
			{	// multiple jumps
				if (newrjumpies.find(p) != newrjumpies.end())
				{
					Operation* newjump = new RJump(*pnewboard);
					if (p->King())
					{
						DirectionOf(p) = UP;
						jump_iterator_func(temp, board, p, newjump);
						DirectionOf(p) = DOWN;
					}
					jump_iterator_func(temp, board, p, newjump);
					delete newjump;
				}
				if (newljumpies.find(p) != newljumpies.end()) 
				{
					Operation* newjump = new LJump(*pnewboard);
					if (p->King())
					{
						DirectionOf(p) = UP;
						jump_iterator_func(temp, board, p, newjump);
						DirectionOf(p) = DOWN;
					}
					jump_iterator_func(temp, board, p, newjump);
					delete newjump;
				}
				delete pnewboard;
			}
		}
		Board* m_pnode;
		bool& m_AIside;
	};
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
			delete m_pAB;
			delete m_pbt;
			m_pAB = nullptr;
			m_pbt = nullptr;
			m_side_set = false;
			m_board.Reset();
		}
		bool IsSideSet() const
		{
			return m_side_set;
		}
		bool AIside() const
		{
			if (!m_side_set)
				throw std::logic_error("AI side is not set");
			return m_AIside;
		}
		Board get_Board() const
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
	template <unsigned int size> class WinBoard
	{
	public:
		WinBoard(CRect* pclient_rect, CClientDC* pDC) :m_br(0, 0, 0, 0)
		{
			// Border rectangle
			int client_x = (pclient_rect->Size()).cx;
			int client_y = (pclient_rect->Size()).cy;

			unsigned int min_size = (client_x < client_y) ? client_x : client_y;
			int vmargin = pDC->GetTextExtent("White").cy;
			m_br_size = min_size - (7 * vmargin);
			int hmargin = (client_x - m_br_size) / 2;
			m_br.right = m_br.bottom = m_br_size;
			m_br.OffsetRect(hmargin, 5 * vmargin);
			m_square_size = m_br_size / size;
			// Letters, numbers
			CSize letter_size = pDC->GetTextExtent("H");
			CSize num_size = pDC->GetTextExtent("6");
			CPoint letter_corner[size];
			CPoint num_corner[size];
			// Horisontal and vertical rectangles
			for (unsigned int i = 0; i < size; ++i)
			{
				m_vrects[i].left = m_br.left + m_square_size * i;
				m_hrects[i].top = m_br.top + m_square_size * i;
				m_vrects[i].right = m_vrects[i].left + m_square_size;
				m_hrects[i].bottom = m_hrects[i].top + m_square_size;
				m_vrects[i].top = m_br.top;
				m_vrects[i].bottom = m_br.bottom;
				m_hrects[i].left = m_br.left;
				m_hrects[i].right = m_br.right;

				num_corner[i] = m_hrects[i].TopLeft();
				num_corner[i].x -= letter_size.cx;
				num_corner[i].y += m_square_size / 2 - num_size.cy / 2;
				m_numbers[i] = CRect(num_corner[i], letter_size);

				letter_corner[i] = m_vrects[i].BottomRight();
				letter_corner[i].x -= (m_square_size/2 + letter_size.cx / 2);
				m_letters[i] = CRect(letter_corner[i], num_size);
			}
		}
		CRect RectAt(char letter, unsigned int num) const
		{
			int vrect_num = letter - 'a';
			int hrect_num = size - num;
			return CRect().IntersectRect(m_vrects + vrect_num, m_hrects + hrect_num);
		}
		CPoint CtrAt(char letter, unsigned int num) const
		{
			return RectAt(letter, num).CenterPoint();
		}
		int SquareSize() const
		{
			return m_square_size;
		}
		void Display(CPaintDC* pDC)
		{
			for (unsigned int i = 0; i < size; ++i)
			{
				pDC->SelectStockObject(HOLLOW_BRUSH);
				pDC->Rectangle(m_vrects + i);
				pDC->SelectStockObject(HOLLOW_BRUSH);
				pDC->Rectangle(m_hrects + i);
				std::string letter, number; 
				letter.push_back('a' + i);
				number.push_back(size - i + '0');
				pDC->DrawTextW(letter.c_str(), m_letters + i, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
				pDC->DrawTextW(number.c_str(), m_numbers + i, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
			}

		}
	private:
		CRect m_br;			// border rectangle
		int m_br_size;		// border rectangle side length
		int m_square_size;
		CRect m_vrects[size];
		CRect m_hrects[size];
		CRect m_letters[size];
		CRect m_numbers[size];
	};
#endif
}