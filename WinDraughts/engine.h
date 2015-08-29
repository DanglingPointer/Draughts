#pragma once
#include<stdexcept>
#include"basic_classes.h"

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
	typedef INodeTool<Field<FSIZE>, char> DraughtsTool;
	typedef DraughtsTool::Value Result;
	typedef DraughtsTool::Node Board;
	typedef MoveFinder<FSIZE> MFinder;
	typedef RightJump<FSIZE> RJump;
	typedef LeftJump<FSIZE> LJump;
	typedef RightMove<FSIZE> RMove;
	typedef LeftMove<FSIZE> LMove;
	typedef std::set<Piece*> pset;

	class BoardTool :public DraughtsTool
	{
	public:
		BoardTool(bool color) : m_pnode(nullptr), m_AIside(color)
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
						DirectionOf(*it) = UP;
					jump_iterator_func(temp, board, *it, pop);
					if ((*it)->King())
					{
						DirectionOf(*it) = DOWN;
						jump_iterator_func(temp, board, *it, pop);
					}
				}
				delete pop;
				pop = new LJump(board);
				for (std::set<Piece*>::const_iterator it = ljumpies.begin(); it != ljumpies.end(); ++it)
				{
					if ((*it)->King())
						DirectionOf(*it) = UP;
					jump_iterator_func(temp, board, *it, pop);
					if ((*it)->King())
					{
						DirectionOf(*it) = DOWN;
						jump_iterator_func(temp, board, *it, pop);
					}
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
						DirectionOf(*it) = UP;
					move_iterator_func(temp, board, *it, pop);
					if ((*it)->King())
					{
						DirectionOf(*it) = DOWN;
						move_iterator_func(temp, board, *it, pop);
					}
				}
				delete pop;
				pop = new LMove(board);
				for (std::set<Piece*>::const_iterator it = lmovies.begin(); it != lmovies.end(); ++it)
				{
					if ((*it)->King())
						DirectionOf(*it) = UP;
					move_iterator_func(temp, board, *it, pop);
					if ((*it)->King())
					{
						DirectionOf(*it) = DOWN;
						move_iterator_func(temp, board, *it, pop);
					}
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
						DirectionOf(p) = UP;
					jump_iterator_func(temp, board, p, newjump);
					if (p->King())
					{
						DirectionOf(p) = DOWN;
						jump_iterator_func(temp, board, p, newjump);
					}
					delete newjump;
				}
				if (newljumpies.find(p) != newljumpies.end()) 
				{
					Operation* newjump = new LJump(*pnewboard);
					if (p->King())
						DirectionOf(p) = UP;
					jump_iterator_func(temp, board, p, newjump);
					if (p->King())
					{
						DirectionOf(p) = DOWN;
						jump_iterator_func(temp, board, p, newjump);
					}
					delete newjump;
				}
				delete pnewboard;
			}
		}
		Board* m_pnode;
		bool m_AIside;
	};
	class Game
	{
	public:
		Field<FSIZE> board;
		Game() :m_side_set(false)
		{ }
		void set_AIside(bool AIside)
		{
			m_side_set = true;
			m_AIside = AIside;
		}
		bool get_AIside() const
		{
			if (!m_side_set)
				throw std::logic_error("AI side is not set");
			return m_AIside;
		}
		void AITurn()
		{
			// To be written
		}
	private:
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
						Direction* pd;
						m_f(letter, num)->White() ? pd = &(dynamic_cast<WhiteKing*>(m_f(letter, num))->dirn) : &(dynamic_cast<BlackKing*>(m_f(letter, num))->dirn);
						(vdirn == 'u') ? *pd = UP : *pd = DOWN;
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
#endif // _CONSOLE
}