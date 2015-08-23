#pragma once
#include"basic_classes.h"
#include<list>
#include<stdexcept>

#ifndef FSIZE
#define FSIZE 8
#endif

namespace Draughts
{
#ifdef _CONSOLE
	class CmdPlay
	{
	public:
		CmdPlay() :m_lj(m_f), m_rj(m_f), m_lm(m_f), m_rm(m_f), m_mf(m_f)
		{ }
		void Run() // no AI so far
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

	#define WIN 1
	#define DRAW 0
	#define LOSS -1
	typedef char res;

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
			if (!m_side_set)
				return;

			MoveFinder<FSIZE> mf(board);
			for (char letter = 'a'; letter < 'a' + FSIZE; ++letter)
				for (unsigned int num = 1; num <= FSIZE; ++num)
				{
					if (board(letter, num) != nullptr && board(letter, num)->White() == m_AIside)
						board(letter, num)->Accept(mf);
				}
			std::set<Piece*> rjumpies = mf.RJumpies();
			std::set<Piece*> ljumpies = mf.LJumpies();

			std::list<Field<FSIZE>> draws;
			if (!rjumpies.empty() || !ljumpies.empty())
			{
				for (std::set<Piece*>::const_iterator it = rjumpies.begin(); it != rjumpies.end(); ++it)
				{
					Field<FSIZE> nboard(board);
					RightJump<FSIZE> rj(nboard);
					nboard(board.Lpos_of(*it), board.Npos_of(*it))->Accept(rj);
					res temp = AlphaBeta(nboard, (!m_AIside) );
					if (temp == WIN)
					{
						board = nboard;
						return;
					}
					if (temp == DRAW) draws.push_back(nboard);
				}
				for (std::set<Piece*>::const_iterator it = ljumpies.begin(); it != ljumpies.end(); ++it)
				{
					Field<FSIZE> nboard(board);
					LeftJump<FSIZE> lj(nboard);
					nboard(board.Lpos_of(*it), board.Npos_of(*it))->Accept(lj);
					res temp = AlphaBeta(nboard, (!m_AIside));
					if (temp == WIN)
					{
						board = nboard;
						return;
					}
					if (temp == DRAW) draws.push_back(nboard);
				}
			}
			else
			{
				std::set<Piece*> rmovies = mf.RMovies();
				std::set<Piece*> lmovies = mf.LMovies();
				for (std::set<Piece*>::const_iterator it = rmovies.begin(); it != rmovies.end(); ++it)
				{
					Field<FSIZE> nboard(board);
					RightMove<FSIZE> rm(nboard);
					nboard(board.Lpos_of(*it), board.Npos_of(*it))->Accept(rm);
					res temp = AlphaBeta(nboard, (!m_AIside));
					if (temp == WIN)
					{
						board = nboard;
						return;
					}
					if (temp == DRAW) draws.push_back(nboard);
				}
				for (std::set<Piece*>::const_iterator it = lmovies.begin(); it != lmovies.end(); ++it)
				{
					Field<FSIZE> nboard(board);
					LeftMove<FSIZE> lm(nboard);
					nboard(board.Lpos_of(*it), board.Npos_of(*it))->Accept(lm);
					res temp = AlphaBeta(nboard, (!m_AIside));
					if (temp == WIN)
					{
						board = nboard;
						return;
					}
					if (temp == DRAW) draws.push_back(nboard);
				}
			} // jump or move
			if (!draws.empty())
				board = draws.back();
		}
	private:
		bool m_AIside;
		bool m_side_set;
		res AlphaBeta(Field<FSIZE>& board, bool turn, res alpha = LOSS, res beta = WIN) // doesn't account for kings!
		{
			bool winning_side;
			if (board.Win(winning_side))
				return ((winning_side == m_AIside) ? WIN : LOSS);
			if (board.Draw())
				return DRAW;
			//-------------------------------------
			MoveFinder<FSIZE> mf(board);
			for (char letter = 'a'; letter < 'a' + FSIZE; ++letter)
				for (unsigned int num = 1; num <= FSIZE; ++num)
				{
					if (board(letter, num) != nullptr && board(letter, num)->White() == turn)
						board(letter, num)->Accept(mf);
				}
			std::set<Piece*> rjumpies = mf.RJumpies();
			std::set<Piece*> ljumpies = mf.LJumpies();
			//-------------------------------------
			if (turn == m_AIside)
			{
				res v = LOSS;
				if (!rjumpies.empty() || !ljumpies.empty())
				{
					for (std::set<Piece*>::const_iterator it = rjumpies.begin(); it != rjumpies.end(); ++it)
					{
						Field<FSIZE> nboard(board);
						RightJump<FSIZE> rj(nboard);
						nboard(board.Lpos_of(*it), board.Npos_of(*it))->Accept(rj);
						res temp = AlphaBeta(nboard, !turn, alpha, beta);
						if (temp > v) v = temp;
						if (v > alpha) alpha = v;
						if (beta <= alpha) return v;
					}
					for (std::set<Piece*>::const_iterator it = ljumpies.begin(); it != ljumpies.end(); ++it)
					{
						Field<FSIZE> nboard(board);
						LeftJump<FSIZE> lj(nboard);
						nboard(board.Lpos_of(*it), board.Npos_of(*it))->Accept(lj);
						res temp = AlphaBeta(nboard, !turn, alpha, beta);
						if (temp > v) v = temp;
						if (v > alpha) alpha = v;
						if (beta <= alpha) return v;
					}
				}
				else
				{
					std::set<Piece*> rmovies = mf.RMovies();
					std::set<Piece*> lmovies = mf.LMovies();
					for (std::set<Piece*>::const_iterator it = rmovies.begin(); it != rmovies.end(); ++it)
					{
						Field<FSIZE> nboard(board);
						RightMove<FSIZE> rm(nboard);
						nboard(board.Lpos_of(*it), board.Npos_of(*it))->Accept(rm);
						res temp = AlphaBeta(nboard, !turn, alpha, beta);
						if (temp > v) v = temp;
						if (v > alpha) alpha = v;
						if (beta <= alpha) return v;
					}
					for (std::set<Piece*>::const_iterator it = lmovies.begin(); it != lmovies.end(); ++it)
					{
						Field<FSIZE> nboard(board);
						LeftMove<FSIZE> lm(nboard);
						nboard(board.Lpos_of(*it), board.Npos_of(*it))->Accept(lm);
						res temp = AlphaBeta(nboard, !turn, alpha, beta);
						if (temp > v) v = temp;
						if (v > alpha) alpha = v;
						if (beta <= alpha) return v;
					}
				} // jump or move
				return v;
			}
			else // if (turn != AIside)
			{
				res v = WIN;
				if (!rjumpies.empty() || !ljumpies.empty())
				{
					for (std::set<Piece*>::const_iterator it = rjumpies.begin(); it != rjumpies.end(); ++it)
					{
						Field<FSIZE> nboard(board);
						RightJump<FSIZE> rj(nboard);
						nboard(board.Lpos_of(*it), board.Npos_of(*it))->Accept(rj);
						res temp = AlphaBeta(nboard, !turn, alpha, beta);
						if (temp < v) v = temp;
						if (v < beta) beta = v;
						if (beta <= alpha) return v;
					}
					for (std::set<Piece*>::const_iterator it = ljumpies.begin(); it != ljumpies.end(); ++it)
					{
						Field<FSIZE> nboard(board);
						LeftJump<FSIZE> lj(nboard);
						nboard(board.Lpos_of(*it), board.Npos_of(*it))->Accept(lj);
						res temp = AlphaBeta(nboard, !turn, alpha, beta);
						if (temp < v) v = temp;
						if (v < beta) beta = v;
						if (beta <= alpha) return v;
					}
				}
				else
				{
					std::set<Piece*> rmovies = mf.RMovies();
					std::set<Piece*> lmovies = mf.LMovies();
					for (std::set<Piece*>::const_iterator it = rmovies.begin(); it != rmovies.end(); ++it)
					{
						Field<FSIZE> nboard(board);
						RightMove<FSIZE> rm(nboard);
						nboard(board.Lpos_of(*it), board.Npos_of(*it))->Accept(rm);
						res temp = AlphaBeta(nboard, !turn, alpha, beta);
						if (temp < v) v = temp;
						if (v < beta) beta = v;
						if (beta <= alpha) return v;
					}
					for (std::set<Piece*>::const_iterator it = lmovies.begin(); it != lmovies.end(); ++it)
					{
						Field<FSIZE> nboard(board);
						LeftMove<FSIZE> lm(nboard);
						nboard(board.Lpos_of(*it), board.Npos_of(*it))->Accept(lm);
						res temp = AlphaBeta(nboard, !turn, alpha, beta);
						if (temp < v) v = temp;
						if (v < beta) beta = v;
						if (beta <= alpha) return v;
					}
				} // jump or move
				return v;
			} // maximizing player
		}
	};
}