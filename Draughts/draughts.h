#pragma once
#include<cctype>
#include<utility>
#include<ostream>
#include<set>

// Rules: https://en.wikipedia.org/wiki/English_draughts

namespace Draughts
{
	class Piece;

	__interface Operation // Visitor interface
	{
		void WhitePiece(Piece* p);
		void BlackPiece(Piece* p);
		void King(Piece* p);
	};
	//-------------------------------------------------------------------------------------------------
	class Piece
	{
	public:
		Piece(bool white, bool king) :m_white(white), m_king(king)
		{ }
		virtual ~Piece()
		{ }
		virtual void Accept(Operation&) = 0;
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
			out << (p.White() ? 'e' : 'o');
		else
			out << (p.White() ? 'ê' : 'ô');
		return out;
	}
	class King :public Piece
	{
	public:
		King(bool white) :Piece(white, true)
		{ }
		void Accept(Operation& op)
		{
			op.King(this);
		}
	};
	class WhitePiece :public Piece
	{
	public:
		WhitePiece() :Piece(true, false)
		{ }
		void Accept(Operation& op)
		{
			op.WhitePiece(this);
		}
	};
	class BlackPiece :public Piece
	{
	public:
		BlackPiece() :Piece(false, false)
		{ }
		void Accept(Operation& op)
		{
			op.BlackPiece(this);
		}
	};
	//-------------------------------------------------------------------------------------------------
	template<unsigned int size> class Field
	{
	public:
		Field()
		{
			for (int i = 0; i < size*size; ++i)
				m_field[i] = nullptr;

			for (unsigned int row = 1; row < size / 2; ++row)
				if (row % 2)
					for (char letter = 'a'; letter < 'a' + size; letter += 2)
						operator()(letter, row) = new WhitePiece;
				else
					for (char letter = 'b'; letter < 'a' + size; letter += 2)
						operator()(letter, row) = new WhitePiece;

			for (unsigned int row = size; row > size / 2 + 1; --row)
				if (row % 2)
					for (char letter = 'a'; letter < 'a' + size; letter += 2)
						operator()(letter, row) = new BlackPiece;
				else
					for (char letter = 'b'; letter < 'a' + size; letter += 2)
						operator()(letter, row) = new BlackPiece;
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
			out << '=';
		out << '\n';
		for (unsigned int npos = fsize; npos > 0; --npos)
		{
			for (char lpos = 'a'; lpos < 'a' + fsize; ++lpos)
				if (f(lpos, npos) != nullptr)
					out << ' ' << *f(lpos, npos);
				else
					out << " -";
			out << '\n';
		}
		out << ' ';
		for (unsigned int i = 0; i < fsize * 2 - 1; ++i)
			out << '=';
		out << '\n';
		return out;
	}
	//------------------------------------------------------------------------------------------------
	template<unsigned int size> class Move_base :public Operation
	{ // Abstract base
	public:
		Move_base(Field<size>& f) :m_board(f)
		{ }
	protected:
		bool movable_right_up(Piece* p)
		{
			return m_board.Inside(m_board.Lpos_of(p) + 1, m_board.Npos_of(p) + 1) && m_board(m_board.Lpos_of(p) + 1, m_board.Npos_of(p) + 1) == nullptr;
		}
		bool movable_left_up(Piece* p)
		{
			return m_board.Inside(m_board.Lpos_of(p) - 1, m_board.Npos_of(p) + 1) && m_board(m_board.Lpos_of(p) - 1, m_board.Npos_of(p) + 1) == nullptr;
		}
		bool movable_right_down(Piece* p)
		{
			return m_board.Inside(m_board.Lpos_of(p) + 1, m_board.Npos_of(p) - 1) && m_board(m_board.Lpos_of(p) + 1, m_board.Npos_of(p) - 1) == nullptr;
		}
		bool movable_left_down(Piece* p)
		{
			return m_board.Inside(m_board.Lpos_of(p) - 1, m_board.Npos_of(p) - 1) && m_board(m_board.Lpos_of(p) - 1, m_board.Npos_of(p) - 1) == nullptr;
		}
		Field<size>& m_board;
	};
	template<unsigned int size> class RightMove :public Move_base < size > 
	{
	public:
		RightMove(Field<size>& f) :Move_base<size>(f)
		{ }
		void WhitePiece(Piece* p)
		{
			if (movable_right_up(p))
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
			if (movable_right_down(p))
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
				steps = (rand() % 2) ? -1 : 1; // stub, determines direction

			if (m_board.Inside(m_board.Lpos_of(p) + 1, m_board.Npos_of(p) + steps) && m_board(m_board.Lpos_of(p) + 1, m_board.Npos_of(p) + steps) == nullptr)
			{
				std::pair<char, unsigned int> temp(m_board.Lpos_of(p), m_board.Npos_of(p));
				m_board(temp.first + 1, temp.second + steps) = p;
				m_board(temp.first, temp.second) = nullptr;
			}
		}
	};
	template<unsigned int size> class LeftMove :public Move_base < size > 
	{
	public:
		LeftMove(Field<size>& f) :Move_base<size>(f)
		{ }
		void WhitePiece(Piece* p)
		{
			if (movable_left_up(p))
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
			if (movable_left_down(p))
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
				std::pair<char, unsigned int> temp(m_board.Lpos_of(p), m_board.Npos_of(p));
				m_board(temp.first - 1, temp.second + steps) = p;
				m_board(temp.first, temp.second) = nullptr;
			}
		}
	};
	template<unsigned int size> class Eat_base :public Operation
	{ // Abstract base
	public:
		Eat_base(Field<size>& f) :m_board(f)
		{ }
	protected:
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
	template<unsigned int size> class LeftEat :public Eat_base < size >
	{
	public:
		LeftEat(Field<size>& f) :Eat_base<size>(f)
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
			if (m_board.Npos_of(p) == 1)
			{
				m_board(m_board.Lpos_of(p), m_board.Npos_of(p)) = new Draughts::King(false);
				p->~Piece();
			}
		}
		void King(Piece* p)
		{
			if (eatable_left_up(p))
			{
				delete m_board(m_board.Lpos_of(p) - 1, m_board.Npos_of(p) + 1);
				m_board(m_board.Lpos_of(p) - 1, m_board.Npos_of(p) + 1) = nullptr;

				std::pair<char, unsigned int> temp(m_board.Lpos_of(p), m_board.Npos_of(p));
				m_board(temp.first - 2, temp.second + 2) = p;
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
		}
	};
	template<unsigned int size> class RightEat :public Eat_base < size >
	{
	public:
		RightEat(Field<size>& f) :Eat_base<size>(f)
		{ }
		void WhitePiece(Piece* p)
		{
			if (eatable_right_up(p))
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
			if (eatable_right_down(p))
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
			if (eatable_right_up(p))
			{
				delete m_board(m_board.Lpos_of(p) + 1, m_board.Npos_of(p) + 1);
				m_board(m_board.Lpos_of(p) + 1, m_board.Npos_of(p) + 1) = nullptr;

				std::pair<char, unsigned int> temp(m_board.Lpos_of(p), m_board.Npos_of(p));
				m_board(temp.first + 2, temp.second + 2) = p;
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
		}
	};

	template<unsigned int size> class MoveFinder :public Eat_base < size >, Move_base < size > 
	{ // Should go through all pieces on one side at a time, then clear buffers
	public:
		MoveFinder(Field<size>& f) :Eat_base<size>(f), Move_base<size>(f)
		{ }
		void WhitePiece(Piece* p)
		{
			if (eatable_right_up(p))
				m_reaties.insert(p);
			else if (eatable_left_up(p))
				m_leaties.insert(p);
			else if (movable_right_up(p))
				m_rmovies.insert(p);
			else if (movable_left_up(p))
				m_lmovies.insert(p);
		}
		void BlackPiece(Piece* p)
		{
			if (eatable_right_down(p))
				m_reaties.insert(p);
			else if (eatable_left_down(p))
				m_leaties.insert(p);
			else if (movable_right_down(p))
				m_rmovies.insert(p);
			else if (movable_left_down(p))
				m_lmovies.insert(p);
		}
		void King(Piece* p)
		{
			if (eatable_right_down(p) || eatable_right_up(p))
				m_reaties.insert(p);
			else if (eatable_left_down(p) || eatable_left_up(p))
				m_leaties.insert(p);
			else if (movable_right_down(p) || movable_right_up(p))
				m_rmovies.insert(p);
			else if (movable_left_down(p) || movable_left_up(p))
				m_lmovies.insert(p);
		}
		std::set<Piece*> REaties() const
		{
			return m_reaties;
		}
		std::set<Piece*> LEaties() const
		{
			return m_leaties;
		}
		std::set<Piece*> RMovies() const
		{
			return m_rmovies;
		}
		std::set<Piece*> LMovies() const
		{
			return m_lmovies;
		}
		void Clear()
		{
			m_eaties.clear();
			m_rmovies.clear();
			m_lmovies.clear();
		}
	private:
		std::set<Piece*> m_reaties;
		std::set<Piece*> m_leaties;
		std::set<Piece*> m_rmovies;
		std::set<Piece*> m_lmovies;
	};
}