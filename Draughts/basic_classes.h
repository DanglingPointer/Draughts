#pragma once
// Rules: https://en.wikipedia.org/wiki/English_draughts

#include<utility>
#include<set>

#ifdef _CONSOLE
#include<iostream>
#endif

#define WHITE true
#define BLACK false
#define KING true
#define MAN false
#define UP 1
#define DOWN -1

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
	protected:
		Piece(bool white, bool king) :m_white(white), m_king(king)
		{ }
	public:
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
#ifdef _CONSOLE
	std::ostream& operator << (std::ostream& out, const Piece& p)
	{
		if (!p.King())
			out << (p.White() ? 'e' : 'o');
		else
			out << (p.White() ? 'ê' : 'ô');
		return out;
	}
#endif
	typedef int Direction;
	template <bool color> class King :public Piece
	{
	public:
		King<color>() :Piece(color, KING)
		{ }
		Direction dirn;
		void Accept(Operation& op)
		{
			op.King(this);
		}
	};
	template <bool color> class Man :public Piece
	{
	public:
		Man<color>():Piece(color, MAN)
		{ }
		void Accept(Operation& op)
		{
			if (White()) op.WhitePiece(this);
			else op.BlackPiece(this);
		}
	};
	typedef King<WHITE> WhiteKing;
	typedef King<BLACK> BlackKing;
	typedef Man<WHITE> WhitePiece;
	typedef Man<BLACK> BlackPiece;
	//-------------------------------------------------------------------------------------------------
	template<unsigned int size> class Field
	{
	public:
		Field<size>()
		{
			Initialize();
		}
		Field<size>(const Field<size>& f)
		{
			Copy(f);
		}
		Field<size>& operator = (const Field<size>& f)
		{
			for (int i = 0; i < size*size; ++i)
				if (*(m_field + i) != nullptr)
					delete *(m_field + i);
			Copy(f);
			return *this;
		}
		~Field<size>()
		{
			for (int i = 0; i < size*size; ++i)
				if (m_field[i] != nullptr)
					delete m_field[i];
		}
		// Lower case letter required
		Piece*& operator() (char letter, unsigned int num)
		{
			return m_field[(num - 1) * size + (letter - 'a')];
		}
		// Lower case letter required
		Piece* operator() (char letter, unsigned int num) const
		{
			return m_field[(num - 1) * size + (letter - 'a')];
		}
		// Lower case letter required
		bool Inside(char letter, unsigned int num) const
		{
			return letter >= 'a' && letter < 'a' + size && num > 0 && num <= size;
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
		bool Win(bool& color) const
		{
			std::set<bool> temp;
			for (int i = 0; i < size*size; ++i)
				if (*(m_field + i) != nullptr)
					temp.insert((*(m_field + i))->White());
			if (temp.size() == 1)
			{
				color = *(temp.begin());
				return true;
			}
			return false;
		}
		bool Draw() const
		{
			unsigned int num_white = 0, num_black = 0;
			for (int i = 0; i < size*size; ++i)
			{
				Piece* p = *(m_field + i);
				if (p != nullptr)
					p->White() ? ++num_white : ++num_black;
			}
			if (num_white == 1 && num_black == 1)
				return true;
			return false;
		}
		void Reset()
		{
			for (int i = 0; i < size*size; ++i)
				if (*(m_field + i) != nullptr)
					delete *(m_field + i);
			Initialize();
		}
	private:
		void Initialize()
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
		void Copy(const Field<size>& f)
		{
			for (int i = 0; i < size*size; ++i)
			{
				if (f.m_field[i] == nullptr)
					m_field[i] = nullptr;
				else if (!f.m_field[i]->King())
					f.m_field[i]->White() ? (m_field[i] = new WhitePiece) : (m_field[i] = new BlackPiece);
				else
					f.m_field[i]->White() ? (m_field[i] = new WhiteKing) : (m_field[i] = new BlackKing);
			}
		}
		Piece* m_field[size*size];
	};
#ifdef _CONSOLE
	template<unsigned int fsize> std::ostream& operator << (std::ostream& out, const Field<fsize>& f)
	{
		out << "    ";
		for (unsigned int i = 0; i < fsize * 2 - 1; ++i)
			out << '=';
		out << '\n';
		for (unsigned int npos = fsize; npos > 0; --npos)
		{
			if (npos < 10) out << ' ';
			out << npos << ' ';
			for (char lpos = 'a'; lpos < 'a' + fsize; ++lpos)
				if (f(lpos, npos) != nullptr)
					out << ' ' << *f(lpos, npos);
				else
					out << " -";
			out << "\n";
		}
		out << "   ";
		for (char lpos = 'a'; lpos < 'a' + fsize; ++lpos)
			out << ' ' << lpos;
		out << "\n    ";
		for (unsigned int i = 0; i < fsize * 2 - 1; ++i)
			out << '=';
		out << '\n';
		return out;
	}
#endif
	//------------------------------------------------------------------------------------------------
	template<unsigned int size> class MoveTool
	{
	public:
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
	protected:
		Field<size>& m_board;
	};
	template<unsigned int size> class JumpTool
	{
	public:
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
	protected:
		Field<size>& m_board;
	};
	//------------------------------------------------------------------------------------------------
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
				m_board(m_board.Lpos_of(p), m_board.Npos_of(p)) = new WhiteKing;
				delete p;
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
				m_board(m_board.Lpos_of(p), m_board.Npos_of(p)) = new BlackKing;
				delete p;
			}
		}
		void King(Piece* p)
		{
			int steps; // either -1 or 1
			steps = (p->White() ? dynamic_cast<WhiteKing*>(p)->dirn : dynamic_cast<BlackKing*>(p)->dirn);

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
				m_board(m_board.Lpos_of(p), m_board.Npos_of(p)) = new WhiteKing;
				delete p;
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
				m_board(m_board.Lpos_of(p), m_board.Npos_of(p)) = new BlackKing;
				delete p;
			}
		}
		void King(Piece* p)
		{
			int steps; // either -1 or 1
			steps = (p->White() ? dynamic_cast<WhiteKing*>(p)->dirn : dynamic_cast<BlackKing*>(p)->dirn);

			if (m_board.Inside(m_board.Lpos_of(p) - 1, m_board.Npos_of(p) + steps) && 
				m_board(m_board.Lpos_of(p) - 1, m_board.Npos_of(p) + steps) == nullptr)
			{
				std::pair<char, unsigned int> temp(m_board.Lpos_of(p), m_board.Npos_of(p));
				m_board(temp.first - 1, temp.second + steps) = p;
				m_board(temp.first, temp.second) = nullptr;
			}
		}
	};
	//------------------------------------------------------------------------------------------------
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
				m_board(m_board.Lpos_of(p), m_board.Npos_of(p)) = new WhiteKing;
				delete p;
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
				m_board(m_board.Lpos_of(p), m_board.Npos_of(p)) = new BlackKing;
				delete p;
			}
		}
		void King(Piece* p)
		{
			Direction direction;
			direction = (p->White() ? dynamic_cast<WhiteKing*>(p)->dirn : dynamic_cast<BlackKing*>(p)->dirn);

			if (direction == UP && jumpable_right_up(p))
			{
				delete m_board(m_board.Lpos_of(p) + 1, m_board.Npos_of(p) + 1);
				m_board(m_board.Lpos_of(p) + 1, m_board.Npos_of(p) + 1) = nullptr;

				std::pair<char, unsigned int> temp(m_board.Lpos_of(p), m_board.Npos_of(p));
				m_board(temp.first + 2, temp.second + 2) = p;
				m_board(temp.first, temp.second) = nullptr;
			}
			else if (direction == DOWN && jumpable_right_down(p))
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
				m_board(m_board.Lpos_of(p), m_board.Npos_of(p)) = new WhiteKing;
				delete p;
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
				m_board(m_board.Lpos_of(p), m_board.Npos_of(p)) = new BlackKing;
				delete p;
			}
		}
		void King(Piece* p)
		{
			Direction direction;
			direction = (p->White() ? dynamic_cast<WhiteKing*>(p)->dirn : dynamic_cast<BlackKing*>(p)->dirn);

			if (direction == UP && jumpable_left_up(p))
			{
				delete m_board(m_board.Lpos_of(p) - 1, m_board.Npos_of(p) + 1);
				m_board(m_board.Lpos_of(p) - 1, m_board.Npos_of(p) + 1) = nullptr;

				std::pair<char, unsigned int> temp(m_board.Lpos_of(p), m_board.Npos_of(p));
				m_board(temp.first - 2, temp.second + 2) = p;
				m_board(temp.first, temp.second) = nullptr;
			}
			else if (direction == DOWN && jumpable_left_down(p))
			{
				delete m_board(m_board.Lpos_of(p) - 1, m_board.Npos_of(p) - 1);
				m_board(m_board.Lpos_of(p) - 1, m_board.Npos_of(p) - 1) = nullptr;

				std::pair<char, unsigned int> temp(m_board.Lpos_of(p), m_board.Npos_of(p));
				m_board(temp.first - 2, temp.second - 2) = p;
				m_board(temp.first, temp.second) = nullptr;
			}
		}
	};
	//------------------------------------------------------------------------------------------------
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
	//------------------------------------------------------------------------------------------------
} // namespace Draughts