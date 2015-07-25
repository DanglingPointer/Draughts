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
				for (char letter = (row % 2) ? 'a' : 'b'; letter < 'a' + size; letter += 2)
					operator()(letter, row) = new WhitePiece;

			for (unsigned int row = size; row > size / 2 + 1; --row)
				for (char letter = (row % 2) ? 'a' : 'b'; letter < 'a' + size; letter += 2)
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
	template<unsigned int size> class MoveTool
	{
	protected:
		MoveTool(Field<size>& f) :m_board(f)
		{ }
		bool movable_right_up(Piece* p) const
		{
			return m_board.Inside(m_board.Lpos_of(p) + 1, m_board.Npos_of(p) + 1) && 
				m_board(m_board.Lpos_of(p) + 1, m_board.Npos_of(p) + 1) == nullptr;
		}
		bool movable_left_up(Piece* p) const
		{
			return m_board.Inside(m_board.Lpos_of(p) - 1, m_board.Npos_of(p) + 1) && 
				m_board(m_board.Lpos_of(p) - 1, m_board.Npos_of(p) + 1) == nullptr;
		}
		bool movable_right_down(Piece* p) const
		{
			return m_board.Inside(m_board.Lpos_of(p) + 1, m_board.Npos_of(p) - 1) && 
				m_board(m_board.Lpos_of(p) + 1, m_board.Npos_of(p) - 1) == nullptr;
		}
		bool movable_left_down(Piece* p) const
		{
			return m_board.Inside(m_board.Lpos_of(p) - 1, m_board.Npos_of(p) - 1) && 
				m_board(m_board.Lpos_of(p) - 1, m_board.Npos_of(p) - 1) == nullptr;
		}
		Field<size>& m_board;
	};
	template<unsigned int size> class RightMove :public Operation, protected MoveTool < size >
	{
	public:
		RightMove(Field<size>& f) :MoveTool<size>(f)
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
	template<unsigned int size> class LeftMove :public Operation, protected MoveTool < size >
	{
	public:
		LeftMove(Field<size>& f) :MoveTool<size>(f)
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
	//------------------------------------------------------------------------------------------------
	template<unsigned int size> class JumpTool
	{
	protected:
		JumpTool(Field<size>& f) :m_board(f)
		{ }
		bool jumpable_right_up(Piece* p) const
		{
			return (
				m_board.Inside(m_board.Lpos_of(p) + 2, m_board.Npos_of(p) + 2) && m_board(m_board.Lpos_of(p) + 2, m_board.Npos_of(p) + 2) == nullptr &&
				m_board(m_board.Lpos_of(p) + 1, m_board.Npos_of(p) + 1) != nullptr && (
				(!m_board(m_board.Lpos_of(p) + 1, m_board.Npos_of(p) + 1)->White() && p->White()) || (m_board(m_board.Lpos_of(p) + 1, m_board.Npos_of(p) + 1)->White() && !p->White())
				));
		}
		bool jumpable_left_up(Piece* p) const
		{
			return (
				m_board.Inside(m_board.Lpos_of(p) - 2, m_board.Npos_of(p) + 2) && m_board(m_board.Lpos_of(p) - 2, m_board.Npos_of(p) + 2) == nullptr &&
				m_board(m_board.Lpos_of(p) - 1, m_board.Npos_of(p) + 1) != nullptr && (
				(!m_board(m_board.Lpos_of(p) - 1, m_board.Npos_of(p) + 1)->White() && p->White()) || (m_board(m_board.Lpos_of(p) - 1, m_board.Npos_of(p) + 1)->White() && !p->White())
				));
		}
		bool jumpable_right_down(Piece* p) const
		{
			return (
				m_board.Inside(m_board.Lpos_of(p) + 2, m_board.Npos_of(p) - 2) && m_board(m_board.Lpos_of(p) + 2, m_board.Npos_of(p) - 2) == nullptr &&
				m_board(m_board.Lpos_of(p) + 1, m_board.Npos_of(p) - 1) != nullptr && (
				(!m_board(m_board.Lpos_of(p) + 1, m_board.Npos_of(p) - 1)->White() && p->White()) || (m_board(m_board.Lpos_of(p) + 1, m_board.Npos_of(p) - 1)->White() && !p->White())
				));
		}
		bool jumpable_left_down(Piece* p) const
		{
			return (
				m_board.Inside(m_board.Lpos_of(p) - 2, m_board.Npos_of(p) - 2) && m_board(m_board.Lpos_of(p) - 2, m_board.Npos_of(p) - 2) == nullptr &&
				m_board(m_board.Lpos_of(p) - 1, m_board.Npos_of(p) - 1) != nullptr && (
				(!m_board(m_board.Lpos_of(p) - 1, m_board.Npos_of(p) - 1)->White() && p->White()) || (m_board(m_board.Lpos_of(p) - 1, m_board.Npos_of(p) - 1)->White() && !p->White())
				));
		}
		Field<size>& m_board;
	};
	template<unsigned int size> class RightJump :public Operation, protected JumpTool < size >
	{
	public:
		RightJump(Field<size>& f) :JumpTool<size>(f)
		{ }
		void WhitePiece(Piece* p)
		{
			if (jumpable_right_up(p))
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
			if (jumpable_right_down(p))
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
			if (jumpable_right_up(p))
			{
				delete m_board(m_board.Lpos_of(p) + 1, m_board.Npos_of(p) + 1);
				m_board(m_board.Lpos_of(p) + 1, m_board.Npos_of(p) + 1) = nullptr;

				std::pair<char, unsigned int> temp(m_board.Lpos_of(p), m_board.Npos_of(p));
				m_board(temp.first + 2, temp.second + 2) = p;
				m_board(temp.first, temp.second) = nullptr;
			}
			else if (jumpable_right_down(p))
			{
				delete m_board(m_board.Lpos_of(p) + 1, m_board.Npos_of(p) - 1);
				m_board(m_board.Lpos_of(p) + 1, m_board.Npos_of(p) - 1) = nullptr;

				std::pair<char, unsigned int> temp(m_board.Lpos_of(p), m_board.Npos_of(p));
				m_board(temp.first + 2, temp.second - 2) = p;
				m_board(temp.first, temp.second) = nullptr;
			}
		}
	};
	template<unsigned int size> class LeftJump :public Operation, protected JumpTool < size >
	{
	public:
		LeftJump(Field<size>& f) :JumpTool<size>(f)
		{ }
		void WhitePiece(Piece* p)
		{
			if (jumpable_left_up(p))
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
			if (jumpable_left_down(p))
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
			if (jumpable_left_up(p))
			{
				delete m_board(m_board.Lpos_of(p) - 1, m_board.Npos_of(p) + 1);
				m_board(m_board.Lpos_of(p) - 1, m_board.Npos_of(p) + 1) = nullptr;

				std::pair<char, unsigned int> temp(m_board.Lpos_of(p), m_board.Npos_of(p));
				m_board(temp.first - 2, temp.second + 2) = p;
				m_board(temp.first, temp.second) = nullptr;
			}
			else if (jumpable_left_down(p))
			{
				delete m_board(m_board.Lpos_of(p) - 1, m_board.Npos_of(p) - 1);
				m_board(m_board.Lpos_of(p) - 1, m_board.Npos_of(p) - 1) = nullptr;

				std::pair<char, unsigned int> temp(m_board.Lpos_of(p), m_board.Npos_of(p));
				m_board(temp.first - 2, temp.second - 2) = p;
				m_board(temp.first, temp.second) = nullptr;
			}
		}
	};

	template<unsigned int size> class MoveFinder :public Operation, protected JumpTool < size >, protected MoveTool < size >
	{ // Should go through all pieces on one side at a time, then clear buffers
	public:
		MoveFinder(Field<size>& f) :JumpTool<size>(f), MoveTool<size>(f)
		{ }
		void WhitePiece(Piece* p)
		{
			if (jumpable_right_up(p) || jumpable_left_up(p))
				m_rjumpies.insert(p);
			if (jumpable_left_up(p)) 
				m_ljumpies.insert(p);

			if (m_rjumpies.empty() && m_ljumpies.empty())
			{
				if (movable_right_up(p))
					m_rmovies.insert(p);
				if (movable_left_up(p))
					m_lmovies.insert(p);
			}
		}
		void BlackPiece(Piece* p)
		{
			if (jumpable_right_down(p))
				m_rjumpies.insert(p);
			if (jumpable_left_down(p))
				m_ljumpies.insert(p);

			if (m_rjumpies.empty() && m_ljumpies.empty())
			{
				if (movable_right_down(p))
					m_rmovies.insert(p);
				if (movable_left_down(p))
					m_lmovies.insert(p);
			}
		}
		void King(Piece* p)
		{
			if (jumpable_right_down(p) || jumpable_right_up(p))
				m_rjumpies.insert(p);
			if (jumpable_left_down(p) || jumpable_left_up(p))
				m_ljumpies.insert(p);

			if (m_rjumpies.empty() && m_ljumpies.empty())
			{
				if (movable_right_down(p) || movable_right_up(p))
					m_rmovies.insert(p);
				if (movable_left_down(p) || movable_left_up(p))
					m_lmovies.insert(p);
			}
		}
		std::set<Piece*> RJumpies() const
		{
			return m_rjumpies;
		}
		std::set<Piece*> LJumpies() const
		{
			return m_ljumpies;
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
			m_rjumpies.clear();
			m_ljumpies.clear();
			m_rmovies.clear();
			m_lmovies.clear();
		}
	private:
		std::set<Piece*> m_rjumpies;	// those in position to jump to the right
		std::set<Piece*> m_ljumpies;
		std::set<Piece*> m_rmovies;
		std::set<Piece*> m_lmovies;		// those in position to move to the left
	};
}