#pragma once
#include<cctype>
#include<utility>
#include<ostream>

namespace Draughts
{
	class Piece;

	__interface Operation // Visitor interface
	{
		void WhitePiece(Piece* p);
		void BlackPiece(Piece* p);
		void King(Piece* p);
	};

	class Piece
	{
	public:
		Piece(bool white, bool king) :m_white(white), m_king(king)
		{ }
		virtual ~Piece()
		{ }

		virtual void Accept(Operation* op) = 0;

		bool King() const
		{
			return m_king;
		}
		bool White() const
		{
			return m_white;
		}
	private:
		bool m_white;
		bool m_king;
	};
	std::ostream& operator << (std::ostream& out, const Piece& p)
	{
		if (!p.King())
		{
			if (p.White())
				out << 'e';
			else
				out << 'o';
		}
		else
		{
			if (p.White())
				out << 'ê';
			else
				out << 'ô';
		}
		return out;
	}

	template<unsigned int size> class Field
	{
	public:
		Field()
		{
			for (int i = 0; i < size*size; ++i)
				m_field[i] = nullptr;
		}
		~Field()
		{
			for (int i = 0; i < size*size; ++i)
				if (m_field[i] != nullptr)
					delete m_field[i];
		}
		Piece*& operator() (char letter, unsigned int num)
		{
			return m_field[(num - 1) * size + (std::tolower(letter) - 'a')];
		}
		Piece* operator() (char letter, unsigned int num) const
		{
			return m_field[(num - 1) * size + (std::tolower(letter) - 'a')];
		}
		bool Inside(char letter, unsigned int num) const
		{
			return std::tolower(letter) >= 'a' && std::tolower(letter) < 'a' + size && num > 0 && num <= size;
		}
		char Lpos_of(Piece* p) const
		{
			for (char letter = 'a'; letter < 'a' + size; ++letter)
				for (unsigned int num = 1; num <= size; ++num)
					if (operator()(letter, num) == p)
						return letter;
			return 0;
		}
		unsigned int Npos_of(Piece* p) const
		{
			for (char letter = 'a'; letter < 'a' + size; ++letter)
				for (unsigned int num = 1; num <= size; ++num)
					if (operator()(letter, num) == p)
						return num;
			return 0;
		}
	private:
		Piece* m_field[size*size];
	};
	template<unsigned int fsize> std::ostream& operator << (std::ostream& out, const Field<fsize>& f)
	{
		out << ' ';
		for (unsigned int i = 0; i < fsize * 2 - 1; ++i)
			out << '-';
		out << '\n';
		for (unsigned int npos = fsize; npos > 0; --npos)
		{
			for (char lpos = 'a'; lpos < 'a' + fsize; ++lpos)
				if (f(lpos, npos) != nullptr)
					out << ' ' << *f(lpos, npos);
				else
					out << " *";
			out << '\n';
		}
		out << ' ';
		for (unsigned int i = 0; i < fsize * 2 - 1; ++i)
			out << '-';
		out << '\n';
		return out;
	}

	//--------------------------------------------------------------------------------------------------

	class King :public Piece
	{
	public:
		King(bool white) :Piece(white, true)
		{ }
		void Accept(Operation* op)
		{
			op->King(this);
		}
	};

	class WhitePiece :public Piece
	{
	public:
		WhitePiece() :Piece(true, false)
		{ }
		~WhitePiece()
		{ }
		void Accept(Operation* op)
		{
			op->WhitePiece(this);
		}
	};

	class BlackPiece :public Piece
	{
	public:
		BlackPiece() :Piece(false, false)
		{ }
		~BlackPiece()
		{ }
		void Accept(Operation* op)
		{
			op->BlackPiece(this);
		}
	};

	//--------------------------------------------------------------------------------------------------

	template<unsigned int size> class RightMove :public Operation
	{
	public:
		RightMove(Field<size>& f) :m_board(f)
		{ }
		void WhitePiece(Piece* p)
		{
			if (m_board.Inside(m_board.Lpos_of(p) + 1, m_board.Npos_of(p) + 1) && m_board(m_board.Lpos_of(p) + 1, m_board.Npos_of(p) + 1) == nullptr)
			{
				std::pair<char, unsigned int> temp(m_board.Lpos_of(p), m_board.Npos_of(p));
				m_board(temp.first + 1, temp.second + 1) = p;
				m_board(temp.first, temp.second) = nullptr;
			}
			if (m_board.Npos_of(p) == size)
			{
				m_board(m_board.Lpos_of(p), m_board.Npos_of(p)) = new Draughts::King(true);
				p->~Piece();
			}
		}
		void BlackPiece(Piece* p)
		{
			if (m_board.Inside(m_board.Lpos_of(p) + 1, m_board.Npos_of(p) - 1) && m_board(m_board.Lpos_of(p) + 1, m_board.Npos_of(p) - 1) == nullptr)
			{
				std::pair<char, unsigned int> temp(m_board.Lpos_of(p), m_board.Npos_of(p));
				m_board(temp.first + 1, temp.second - 1) = p;
				m_board(temp.first, temp.second) = nullptr;
			}
			if (m_board.Npos_of(p) == 1)
			{
				m_board(m_board.Lpos_of(p), m_board.Npos_of(p)) = new Draughts::King(false);
				p->~Piece();
			}
		}
		void King(Piece* p)
		{
			int steps;
			if (p->White())
				steps = (rand() % 2) ? -1 : 1;// stub, determines direction
			else
				steps = (rand() % 2) ? -1 : 1; // stub

			if (m_board.Inside(m_board.Lpos_of(p) + 1, m_board.Npos_of(p) + steps) && m_board(m_board.Lpos_of(p) + 1, m_board.Npos_of(p) + steps) == nullptr)
			{
				m_board(m_board.Lpos_of(p) + 1, m_board.Npos_of(p) + steps) = p;
				m_board(m_board.Lpos_of(p) - 1, m_board.Npos_of(p) - steps) = nullptr;
			}
		}
	private:
		Field<size>& m_board;
	};

	template<unsigned int size> class LeftMove :public Operation
	{
	public:
		LeftMove(Field<size>& f) :m_board(f)
		{ }
		void WhitePiece(Piece* p)
		{
			if (m_board.Inside(m_board.Lpos_of(p) - 1, m_board.Npos_of(p) + 1) && m_board(m_board.Lpos_of(p) - 1, m_board.Npos_of(p) + 1) == nullptr)
			{
				std::pair<char, unsigned int> temp(m_board.Lpos_of(p), m_board.Npos_of(p));
				m_board(temp.first - 1, temp.second + 1) = p;
				m_board(temp.first, temp.second) = nullptr;
			}
			if (m_board.Npos_of(p) == size)
			{
				m_board(m_board.Lpos_of(p), m_board.Npos_of(p)) = new Draughts::King(true);
				p->~Piece();
			}
		}
		void BlackPiece(Piece* p)
		{
			if (m_board.Inside(m_board.Lpos_of(p) - 1, m_board.Npos_of(p) - 1) && m_board(m_board.Lpos_of(p) - 1, m_board.Npos_of(p) - 1) == nullptr)
			{
				std::pair<char, unsigned int> temp(m_board.Lpos_of(p), m_board.Npos_of(p));
				m_board(temp.first - 1, temp.second - 1) = p;
				m_board(temp.first, temp.second) = nullptr;
			}
			if (m_board.Npos_of(p) == 1)
			{
				m_board(m_board.Lpos_of(p), m_board.Npos_of(p)) = new Draughts::King(false);
				p->~Piece();
			}
		}
		void King(Piece* p)
		{
			int steps; // either -1 or 1
			if (p->White())
				steps = (rand() % 2) ? -1 : 1; // stub, determines direction
			else
				steps = (rand() % 2) ? -1 : 1; // stub, determines direction

			if (m_board.Inside(m_board.Lpos_of(p) - 1, m_board.Npos_of(p) + steps) && m_board(m_board.Lpos_of(p) - 1, m_board.Npos_of(p) + steps) == nullptr)
			{
				m_board(m_board.Lpos_of(p) - 1, m_board.Npos_of(p) + steps) = p;
				m_board(m_board.Lpos_of(p) + 1, m_board.Npos_of(p) - steps) = nullptr;
			}
		}
	private:
		Field<size>& m_board;
	};

	template<unsigned int size> class Eat :public Operation
	{
	public:
		Eat(Field<size>& f) :m_board(f)
		{ }
		void WhitePiece(Piece* p)
		{
			if (eatable_left_up(p))
			{
				delete m_board(m_board.Lpos_of(p) - 1, m_board.Npos_of(p) + 1);
				m_board(m_board.Lpos_of(p) - 1, m_board.Npos_of(p) + 1) = nullptr;

				std::pair<char, unsigned int> temp(m_board.Lpos_of(p), m_board.Npos_of(p));
				m_board(temp.first - 2, temp.second + 2) = p;
				m_board(temp.first, temp.second) = nullptr;
			}
			else if (eatable_right_up(p))
			{
				delete m_board(m_board.Lpos_of(p) + 1, m_board.Npos_of(p) + 1);
				m_board(m_board.Lpos_of(p) + 1, m_board.Npos_of(p) + 1) = nullptr;

				std::pair<char, unsigned int> temp(m_board.Lpos_of(p), m_board.Npos_of(p));
				m_board(temp.first + 2, temp.second + 2) = p;
				m_board(temp.first, temp.second) = nullptr;
			}
			if (m_board.Npos_of(p) == size)
			{
				m_board(m_board.Lpos_of(p), m_board.Npos_of(p)) = new Draughts::King(true);
				p->~Piece();
			}
		}
		void BlackPiece(Piece* p)
		{
			if (eatable_left_down(p))
			{
				delete m_board(m_board.Lpos_of(p) - 1, m_board.Npos_of(p) - 1);
				m_board(m_board.Lpos_of(p) - 1, m_board.Npos_of(p) - 1) = nullptr;

				std::pair<char, unsigned int> temp(m_board.Lpos_of(p), m_board.Npos_of(p));
				m_board(temp.first - 2, temp.second - 2) = p;
				m_board(temp.first, temp.second) = nullptr;
			}
			else if (eatable_right_down(p))
			{
				delete m_board(m_board.Lpos_of(p) + 1, m_board.Npos_of(p) - 1);
				m_board(m_board.Lpos_of(p) + 1, m_board.Npos_of(p) - 1) = nullptr;

				std::pair<char, unsigned int> temp(m_board.Lpos_of(p), m_board.Npos_of(p));
				m_board(temp.first + 2, temp.second - 2) = p;
				m_board(temp.first, temp.second) = nullptr;
			}
			if (m_board.Npos_of(p) == 1)
			{
				m_board(m_board.Lpos_of(p), m_board.Npos_of(p)) = new Draughts::King(false);
				p->~Piece();
			}
		}
		void King(Piece* p)
		{
			while (eatable_left_down(p) || eatable_left_up(p) || eatable_right_down(p) || eatable_right_up(p))
			{
				if (eatable_left_up(p))
				{
					delete m_board(m_board.Lpos_of(p) - 1, m_board.Npos_of(p) + 1);
					m_board(m_board.Lpos_of(p) - 1, m_board.Npos_of(p) + 1) = nullptr;

					std::pair<char, unsigned int> temp(m_board.Lpos_of(p), m_board.Npos_of(p));
					m_board(temp.first - 2, temp.second + 2) = p;
					m_board(temp.first, temp.second) = nullptr;
				}
				else if (eatable_right_up(p))
				{
					delete m_board(m_board.Lpos_of(p) + 1, m_board.Npos_of(p) + 1);
					m_board(m_board.Lpos_of(p) + 1, m_board.Npos_of(p) + 1) = nullptr;

					std::pair<char, unsigned int> temp(m_board.Lpos_of(p), m_board.Npos_of(p));
					m_board(temp.first + 2, temp.second + 2) = p;
					m_board(temp.first, temp.second) = nullptr;
				}
				else if (eatable_left_down(p))
				{
					delete m_board(m_board.Lpos_of(p) - 1, m_board.Npos_of(p) - 1);
					m_board(m_board.Lpos_of(p) - 1, m_board.Npos_of(p) - 1) = nullptr;

					std::pair<char, unsigned int> temp(m_board.Lpos_of(p), m_board.Npos_of(p));
					m_board(temp.first - 2, temp.second - 2) = p;
					m_board(temp.first, temp.second) = nullptr;
				}
				else
				{
					delete m_board(m_board.Lpos_of(p) + 1, m_board.Npos_of(p) - 1);
					m_board(m_board.Lpos_of(p) + 1, m_board.Npos_of(p) - 1) = nullptr;

					std::pair<char, unsigned int> temp(m_board.Lpos_of(p), m_board.Npos_of(p));
					m_board(temp.first + 2, temp.second - 2) = p;
					m_board(temp.first, temp.second) = nullptr;
				}
			}
		}
	private:
		Field<size>& m_board;

		bool eatable_right_up(Piece* p) const
		{
			return (
				m_board.Inside(m_board.Lpos_of(p) + 2, m_board.Npos_of(p) + 2) && m_board(m_board.Lpos_of(p) + 2, m_board.Npos_of(p) + 2) == nullptr &&
				m_board(m_board.Lpos_of(p) + 1, m_board.Npos_of(p) + 1) != nullptr && (
				(!m_board(m_board.Lpos_of(p) + 1, m_board.Npos_of(p) + 1)->White() && p->White()) || (m_board(m_board.Lpos_of(p) + 1, m_board.Npos_of(p) + 1)->White() && !p->White())
				));
		}
		bool eatable_left_up(Piece* p) const
		{
			return (
				m_board.Inside(m_board.Lpos_of(p) - 2, m_board.Npos_of(p) + 2) && m_board(m_board.Lpos_of(p) - 2, m_board.Npos_of(p) + 2) == nullptr &&
				m_board(m_board.Lpos_of(p) - 1, m_board.Npos_of(p) + 1) != nullptr && (
				(!m_board(m_board.Lpos_of(p) - 1, m_board.Npos_of(p) + 1)->White() && p->White()) || (m_board(m_board.Lpos_of(p) - 1, m_board.Npos_of(p) + 1)->White() && !p->White())
				));
		}
		bool eatable_right_down(Piece* p) const
		{
			return (
				m_board.Inside(m_board.Lpos_of(p) + 2, m_board.Npos_of(p) - 2) && m_board(m_board.Lpos_of(p) + 2, m_board.Npos_of(p) - 2) == nullptr &&
				m_board(m_board.Lpos_of(p) + 1, m_board.Npos_of(p) - 1) != nullptr && (
				(!m_board(m_board.Lpos_of(p) + 1, m_board.Npos_of(p) - 1)->White() && p->White()) || (m_board(m_board.Lpos_of(p) + 1, m_board.Npos_of(p) - 1)->White() && !p->White())
				));
		}
		bool eatable_left_down(Piece* p) const
		{
			return (
				m_board.Inside(m_board.Lpos_of(p) - 2, m_board.Npos_of(p) - 2) && m_board(m_board.Lpos_of(p) - 2, m_board.Npos_of(p) - 2) == nullptr &&
				m_board(m_board.Lpos_of(p) - 1, m_board.Npos_of(p) - 1) != nullptr && (
				(!m_board(m_board.Lpos_of(p) - 1, m_board.Npos_of(p) - 1)->White() && p->White()) || (m_board(m_board.Lpos_of(p) - 1, m_board.Npos_of(p) - 1)->White() && !p->White())
				));
		}
	};
}