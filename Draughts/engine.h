#pragma once
#include"basic_classes.h"

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
}