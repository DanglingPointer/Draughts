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

#define DirectionOf(ppiece) ((ppiece)->White() ? dynamic_cast<WhiteKing*>((ppiece))->dirn : dynamic_cast<BlackKing*>((ppiece))->dirn)

namespace Draughts
{
	typedef bool color;
	class Piece;
	__interface IField // Board of variable size
	{
		Piece*& at(char, unsigned int);
		Piece* at(char, unsigned int) const;
		unsigned int Size() const;
		bool Inside(char, unsigned int) const;
		char Lpos_of(Piece*) const;
		unsigned int Npos_of(Piece*) const;
		bool Win(color&) const;
		bool Draw() const;
		void Reset();
	};
	__interface IOperation // Visitor
	{
		void WhitePiece(Piece*);
		void BlackPiece(Piece*);
		void King(Piece*);
		void set_Board(IField*);
	};
	//-------------------------------------------------------------------------------------------------
	class Piece
	{
	protected:
		Piece(color white, bool king) :m_white(white), m_king(king)
		{ }
	public:
		virtual ~Piece()
		{ }
		virtual void Accept(IOperation&) = 0;
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
	template <color color> class King :public Piece
	{
	public:
		King<color>() :Piece(color, KING)
		{ }
		Direction dirn;
		void Accept(IOperation& op)
		{
			op.King(this);
		}
	};
	template <color color> class Man :public Piece
	{
	public:
		Man<color>():Piece(color, MAN)
		{ }
		void Accept(IOperation& op)
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
	template<unsigned int size> class Field :public IField
	{
	public:
		Field<size>()
		{
			Initialize();
		}
		Field<size>(const Field<size>& f)
		{
			for (int i = 0; i < size*size; ++i)
			{
				if (f.m_field[i] == nullptr)
					m_field[i] = nullptr;
				else if (!(f.m_field[i]->King()))
					f.m_field[i]->White() ? (m_field[i] = new WhitePiece) : (m_field[i] = new BlackPiece);
				else
					f.m_field[i]->White() ? (m_field[i] = new WhiteKing) : (m_field[i] = new BlackKing);
			}
		}
		explicit Field<size>(IField* pf) // size must be equal
		{
			for (unsigned int num = 1; num <= size; ++num)
				for (char letter = 'a'; letter < 'a' + size; ++letter)
				{
					if (pf->at(letter, num) == nullptr)
						at(letter, num) = nullptr;
					else if (!(pf->at(letter, num)->King()))
						pf->at(letter, num)->White() ? (at(letter, num) = new WhitePiece) : (at(letter, num) = new BlackPiece);
					else
						pf->at(letter, num)->White() ? (at(letter, num) = new WhiteKing) : (at(letter, num) = new BlackKing);
				}
		}
		Field<size>(const IField&) = delete;
		Field<size>& operator = (const Field<size>& f) = delete;
		Field<size>& operator = (const IField& f) = delete;
		~Field<size>()
		{
			for (int i = 0; i < size*size; ++i)
				if (m_field[i] != nullptr)
					delete m_field[i];
		}
		// Lower case letter required
		Piece*& at(char letter, unsigned int num)
		{
			return m_field[(num - 1) * size + (letter - 'a')];
		}
		// Lower case letter required
		Piece* at(char letter, unsigned int num) const
		{
			return m_field[(num - 1) * size + (letter - 'a')];
		}
		unsigned int Size() const
		{
			return size;
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
					if (at(letter, num) == p)
						return letter;
			return 0;
		}
		unsigned int Npos_of(Piece* p) const
		{
			for (char letter = 'a'; letter < 'a' + size; ++letter)
				for (unsigned int num = 1; num <= size; ++num)
					if (at(letter, num) == p)
						return num;
			return 0;
		}
		bool Win(color& color) const
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
					(p->White()) ? (++num_white) : (++num_black);
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
					at(letter, row) = new WhitePiece;

			for (unsigned int row = size; row > size / 2 + 1; --row)
				for (char letter = (row % 2) ? 'a' : 'b'; letter < 'a' + size; letter += 2)
					at(letter, row) = new BlackPiece;
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
				if (f.at(lpos, npos) != nullptr)
					out << ' ' << *(f.at(lpos, npos));
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
	class MoveTool
	{
	public:
		MoveTool(IField* pf) :m_pboard(pf)
		{ }
		bool movable_right_up(Piece* p) const
		{
			return m_pboard->Inside(m_pboard->Lpos_of(p) + 1, m_pboard->Npos_of(p) + 1) && 
				m_pboard->at(m_pboard->Lpos_of(p) + 1, m_pboard->Npos_of(p) + 1) == nullptr;
		}
		bool movable_left_up(Piece* p) const
		{
			return m_pboard->Inside(m_pboard->Lpos_of(p) - 1, m_pboard->Npos_of(p) + 1) && 
				m_pboard->at(m_pboard->Lpos_of(p) - 1, m_pboard->Npos_of(p) + 1) == nullptr;
		}
		bool movable_right_down(Piece* p) const
		{
			return m_pboard->Inside(m_pboard->Lpos_of(p) + 1, m_pboard->Npos_of(p) - 1) && 
				m_pboard->at(m_pboard->Lpos_of(p) + 1, m_pboard->Npos_of(p) - 1) == nullptr;
		}
		bool movable_left_down(Piece* p) const
		{
			return m_pboard->Inside(m_pboard->Lpos_of(p) - 1, m_pboard->Npos_of(p) - 1) && 
				m_pboard->at(m_pboard->Lpos_of(p) - 1, m_pboard->Npos_of(p) - 1) == nullptr;
		}
	protected:
		IField* m_pboard;
	};
	class JumpTool
	{
	public:
		JumpTool(IField* pf) :m_pboard(pf)
		{ }
		bool jumpable_right_up(Piece* p) const
		{
			return (
				m_pboard->Inside(m_pboard->Lpos_of(p) + 2, m_pboard->Npos_of(p) + 2) && m_pboard->at(m_pboard->Lpos_of(p) + 2, m_pboard->Npos_of(p) + 2) == nullptr &&
				m_pboard->at(m_pboard->Lpos_of(p) + 1, m_pboard->Npos_of(p) + 1) != nullptr && (
				(!m_pboard->at(m_pboard->Lpos_of(p) + 1, m_pboard->Npos_of(p) + 1)->White() && p->White()) || (m_pboard->at(m_pboard->Lpos_of(p) + 1, m_pboard->Npos_of(p) + 1)->White() && !p->White())
				));
		}
		bool jumpable_left_up(Piece* p) const
		{
			return (
				m_pboard->Inside(m_pboard->Lpos_of(p) - 2, m_pboard->Npos_of(p) + 2) && m_pboard->at(m_pboard->Lpos_of(p) - 2, m_pboard->Npos_of(p) + 2) == nullptr &&
				m_pboard->at(m_pboard->Lpos_of(p) - 1, m_pboard->Npos_of(p) + 1) != nullptr && (
				(!m_pboard->at(m_pboard->Lpos_of(p) - 1, m_pboard->Npos_of(p) + 1)->White() && p->White()) || (m_pboard->at(m_pboard->Lpos_of(p) - 1, m_pboard->Npos_of(p) + 1)->White() && !p->White())
				));
		}
		bool jumpable_right_down(Piece* p) const
		{
			return (
				m_pboard->Inside(m_pboard->Lpos_of(p) + 2, m_pboard->Npos_of(p) - 2) && m_pboard->at(m_pboard->Lpos_of(p) + 2, m_pboard->Npos_of(p) - 2) == nullptr &&
				m_pboard->at(m_pboard->Lpos_of(p) + 1, m_pboard->Npos_of(p) - 1) != nullptr && (
				(!m_pboard->at(m_pboard->Lpos_of(p) + 1, m_pboard->Npos_of(p) - 1)->White() && p->White()) || (m_pboard->at(m_pboard->Lpos_of(p) + 1, m_pboard->Npos_of(p) - 1)->White() && !p->White())
				));
		}
		bool jumpable_left_down(Piece* p) const
		{
			return (
				m_pboard->Inside(m_pboard->Lpos_of(p) - 2, m_pboard->Npos_of(p) - 2) && m_pboard->at(m_pboard->Lpos_of(p) - 2, m_pboard->Npos_of(p) - 2) == nullptr &&
				m_pboard->at(m_pboard->Lpos_of(p) - 1, m_pboard->Npos_of(p) - 1) != nullptr && (
				(!m_pboard->at(m_pboard->Lpos_of(p) - 1, m_pboard->Npos_of(p) - 1)->White() && p->White()) || (m_pboard->at(m_pboard->Lpos_of(p) - 1, m_pboard->Npos_of(p) - 1)->White() && !p->White())
				));
		}
	protected:
		IField* m_pboard;
	};
	//------------------------------------------------------------------------------------------------
	class RightMove :public IOperation, protected MoveTool
	{
	public:
		RightMove(IField* pf = nullptr) :MoveTool(pf)
		{ }
		void WhitePiece(Piece* p)
		{
			if (movable_right_up(p))
			{
				std::pair<char, unsigned int> temp(m_pboard->Lpos_of(p), m_pboard->Npos_of(p));
				m_pboard->at(temp.first + 1, temp.second + 1) = p;
				m_pboard->at(temp.first, temp.second) = nullptr;
			}
			if (m_pboard->Npos_of(p) == m_pboard->Size())
			{
				m_pboard->at(m_pboard->Lpos_of(p), m_pboard->Npos_of(p)) = new WhiteKing;
				delete p;
			}
		}
		void BlackPiece(Piece* p)
		{
			if (movable_right_down(p))
			{
				std::pair<char, unsigned int> temp(m_pboard->Lpos_of(p), m_pboard->Npos_of(p));
				m_pboard->at(temp.first + 1, temp.second - 1) = p;
				m_pboard->at(temp.first, temp.second) = nullptr;
			}
			if (m_pboard->Npos_of(p) == 1)
			{
				m_pboard->at(m_pboard->Lpos_of(p), m_pboard->Npos_of(p)) = new BlackKing;
				delete p;
			}
		}
		void King(Piece* p)
		{
			int steps = DirectionOf(p);

			if (m_pboard->Inside(m_pboard->Lpos_of(p) + 1, m_pboard->Npos_of(p) + steps) && m_pboard->at(m_pboard->Lpos_of(p) + 1, m_pboard->Npos_of(p) + steps) == nullptr)
			{
				std::pair<char, unsigned int> temp(m_pboard->Lpos_of(p), m_pboard->Npos_of(p));
				m_pboard->at(temp.first + 1, temp.second + steps) = p;
				m_pboard->at(temp.first, temp.second) = nullptr;
			}
		}
		void set_Board(IField* pf)
		{
			m_pboard = pf;
		}
	};
	class LeftMove :public IOperation, protected MoveTool
	{
	public:
		LeftMove(IField* pf = nullptr) :MoveTool(pf)
		{ }
		void WhitePiece(Piece* p)
		{
			if (movable_left_up(p))
			{
				std::pair<char, unsigned int> temp(m_pboard->Lpos_of(p), m_pboard->Npos_of(p));
				m_pboard->at(temp.first - 1, temp.second + 1) = p;
				m_pboard->at(temp.first, temp.second) = nullptr;
			}
			if (m_pboard->Npos_of(p) == m_pboard->Size())
			{
				m_pboard->at(m_pboard->Lpos_of(p), m_pboard->Npos_of(p)) = new WhiteKing;
				delete p;
			}
		}
		void BlackPiece(Piece* p)
		{
			if (movable_left_down(p))
			{
				std::pair<char, unsigned int> temp(m_pboard->Lpos_of(p), m_pboard->Npos_of(p));
				m_pboard->at(temp.first - 1, temp.second - 1) = p;
				m_pboard->at(temp.first, temp.second) = nullptr;
			}
			if (m_pboard->Npos_of(p) == 1)
			{
				m_pboard->at(m_pboard->Lpos_of(p), m_pboard->Npos_of(p)) = new BlackKing;
				delete p;
			}
		}
		void King(Piece* p)
		{
			int steps = DirectionOf(p);

			if (m_pboard->Inside(m_pboard->Lpos_of(p) - 1, m_pboard->Npos_of(p) + steps) && 
				m_pboard->at(m_pboard->Lpos_of(p) - 1, m_pboard->Npos_of(p) + steps) == nullptr)
			{
				std::pair<char, unsigned int> temp(m_pboard->Lpos_of(p), m_pboard->Npos_of(p));
				m_pboard->at(temp.first - 1, temp.second + steps) = p;
				m_pboard->at(temp.first, temp.second) = nullptr;
			}
		}
		void set_Board(IField* pf)
		{
			m_pboard = pf;
		}
	};
	//------------------------------------------------------------------------------------------------
	class RightJump :public IOperation, protected JumpTool
	{
	public:
		RightJump(IField* pf = nullptr) :JumpTool(pf)
		{ }
		void WhitePiece(Piece* p)
		{
			if (jumpable_right_up(p))
			{
				delete m_pboard->at(m_pboard->Lpos_of(p) + 1, m_pboard->Npos_of(p) + 1);
				m_pboard->at(m_pboard->Lpos_of(p) + 1, m_pboard->Npos_of(p) + 1) = nullptr;

				std::pair<char, unsigned int> temp(m_pboard->Lpos_of(p), m_pboard->Npos_of(p));
				m_pboard->at(temp.first + 2, temp.second + 2) = p;
				m_pboard->at(temp.first, temp.second) = nullptr;
			}
			if (m_pboard->Npos_of(p) == m_pboard->Size())
			{
				m_pboard->at(m_pboard->Lpos_of(p), m_pboard->Npos_of(p)) = new WhiteKing;
				delete p;
			}
		}
		void BlackPiece(Piece* p)
		{
			if (jumpable_right_down(p))
			{
				delete m_pboard->at(m_pboard->Lpos_of(p) + 1, m_pboard->Npos_of(p) - 1);
				m_pboard->at(m_pboard->Lpos_of(p) + 1, m_pboard->Npos_of(p) - 1) = nullptr;

				std::pair<char, unsigned int> temp(m_pboard->Lpos_of(p), m_pboard->Npos_of(p));
				m_pboard->at(temp.first + 2, temp.second - 2) = p;
				m_pboard->at(temp.first, temp.second) = nullptr;
			}
			if (m_pboard->Npos_of(p) == 1)
			{
				m_pboard->at(m_pboard->Lpos_of(p), m_pboard->Npos_of(p)) = new BlackKing;
				delete p;
			}
		}
		void King(Piece* p)
		{
			Direction direction = DirectionOf(p);

			if (direction == UP && jumpable_right_up(p))
			{
				delete m_pboard->at(m_pboard->Lpos_of(p) + 1, m_pboard->Npos_of(p) + 1);
				m_pboard->at(m_pboard->Lpos_of(p) + 1, m_pboard->Npos_of(p) + 1) = nullptr;

				std::pair<char, unsigned int> temp(m_pboard->Lpos_of(p), m_pboard->Npos_of(p));
				m_pboard->at(temp.first + 2, temp.second + 2) = p;
				m_pboard->at(temp.first, temp.second) = nullptr;
			}
			else if (direction == DOWN && jumpable_right_down(p))
			{
				delete m_pboard->at(m_pboard->Lpos_of(p) + 1, m_pboard->Npos_of(p) - 1);
				m_pboard->at(m_pboard->Lpos_of(p) + 1, m_pboard->Npos_of(p) - 1) = nullptr;

				std::pair<char, unsigned int> temp(m_pboard->Lpos_of(p), m_pboard->Npos_of(p));
				m_pboard->at(temp.first + 2, temp.second - 2) = p;
				m_pboard->at(temp.first, temp.second) = nullptr;
			}
		}
		void set_Board(IField* pf)
		{
			m_pboard = pf;
		}
	};
	class LeftJump :public IOperation, protected JumpTool
	{
	public:
		LeftJump(IField* pf = nullptr) :JumpTool(pf)
		{ }
		void WhitePiece(Piece* p)
		{
			if (jumpable_left_up(p))
			{
				delete m_pboard->at(m_pboard->Lpos_of(p) - 1, m_pboard->Npos_of(p) + 1);
				m_pboard->at(m_pboard->Lpos_of(p) - 1, m_pboard->Npos_of(p) + 1) = nullptr;

				std::pair<char, unsigned int> temp(m_pboard->Lpos_of(p), m_pboard->Npos_of(p));
				m_pboard->at(temp.first - 2, temp.second + 2) = p;
				m_pboard->at(temp.first, temp.second) = nullptr;
			}
			if (m_pboard->Npos_of(p) == m_pboard->Size())
			{
				m_pboard->at(m_pboard->Lpos_of(p), m_pboard->Npos_of(p)) = new WhiteKing;
				delete p;
			}
		}
		void BlackPiece(Piece* p)
		{
			if (jumpable_left_down(p))
			{
				delete m_pboard->at(m_pboard->Lpos_of(p) - 1, m_pboard->Npos_of(p) - 1);
				m_pboard->at(m_pboard->Lpos_of(p) - 1, m_pboard->Npos_of(p) - 1) = nullptr;

				std::pair<char, unsigned int> temp(m_pboard->Lpos_of(p), m_pboard->Npos_of(p));
				m_pboard->at(temp.first - 2, temp.second - 2) = p;
				m_pboard->at(temp.first, temp.second) = nullptr;
			}
			if (m_pboard->Npos_of(p) == 1)
			{
				m_pboard->at(m_pboard->Lpos_of(p), m_pboard->Npos_of(p)) = new BlackKing;
				delete p;
			}
		}
		void King(Piece* p)
		{
			Direction direction = DirectionOf(p);

			if (direction == UP && jumpable_left_up(p))
			{
				delete m_pboard->at(m_pboard->Lpos_of(p) - 1, m_pboard->Npos_of(p) + 1);
				m_pboard->at(m_pboard->Lpos_of(p) - 1, m_pboard->Npos_of(p) + 1) = nullptr;

				std::pair<char, unsigned int> temp(m_pboard->Lpos_of(p), m_pboard->Npos_of(p));
				m_pboard->at(temp.first - 2, temp.second + 2) = p;
				m_pboard->at(temp.first, temp.second) = nullptr;
			}
			else if (direction == DOWN && jumpable_left_down(p))
			{
				delete m_pboard->at(m_pboard->Lpos_of(p) - 1, m_pboard->Npos_of(p) - 1);
				m_pboard->at(m_pboard->Lpos_of(p) - 1, m_pboard->Npos_of(p) - 1) = nullptr;

				std::pair<char, unsigned int> temp(m_pboard->Lpos_of(p), m_pboard->Npos_of(p));
				m_pboard->at(temp.first - 2, temp.second - 2) = p;
				m_pboard->at(temp.first, temp.second) = nullptr;
			}
		}
		void set_Board(IField* pf)
		{
			m_pboard = pf;
		}
	};
	//------------------------------------------------------------------------------------------------
	class MoveFinder :public IOperation, protected JumpTool, protected MoveTool
	{ // Should go through all pieces on one side at a time, then clear buffers
	public:
		MoveFinder(IField* pf) :JumpTool(pf), MoveTool(pf)
		{ }
		void WhitePiece(Piece* p)
		{
			if (jumpable_right_up(p))
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
		void set_Board(IField* pf)
		{
			JumpTool::m_pboard = MoveTool::m_pboard = pf;
			Clear();
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