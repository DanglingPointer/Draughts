#pragma once
#define SIZE 8
#include<cctype>
#include<utility>
#include<ostream>

class Field;

class Piece
{
public:
	Piece(char letter, unsigned int num, Field* pboard, bool white, bool king)
		:m_lpos(letter), m_npos(num), m_board(*pboard), m_white(white), m_king(king)
	{}
	virtual ~Piece()
	{}

	virtual void move_right() = 0;
	virtual void move_left() = 0;
	virtual bool eat() = 0;
	
	bool king() const
	{
		return m_king;
	}
	bool white() const
	{
		return m_white;
	}
	std::pair<char, unsigned int> pos() const
	{
		return std::pair<char, unsigned int>{m_lpos, m_npos};
	}
protected:
	Field& m_board;
	char m_lpos;
	unsigned int m_npos;
private:
	bool m_white;
	bool m_king;
};

class Field
{
public:
	Field()
	{
		for (int i = 0; i < SIZE*SIZE; ++i)
			m_field[i] = nullptr;
	}
	~Field()
	{
		for (int i = 0; i < SIZE*SIZE; ++i)
			if (m_field[i] != nullptr)
				delete m_field[i];
	}
	Piece*& operator() (char letter, unsigned int num)
	{
		return m_field[(num - 1) * SIZE + (std::tolower(letter) - 'a')];
	}
	Piece* operator() (char letter, unsigned int num) const
	{
		return m_field[(num - 1) * SIZE + (std::tolower(letter) - 'a')];
	}
	bool inside(char letter, unsigned int num)
	{
		return letter >= 'a' && letter < 'a' + SIZE && num > 0 && num <= SIZE;
	}
private:
	Piece* m_field[SIZE*SIZE];
};
std::ostream& operator << (std::ostream& out, const Field& f)
{
	for (int npos = SIZE; npos > 0; --npos)
	{
		for (char lpos = 'a'; lpos < 'a' + SIZE; ++lpos)
			out << f(lpos, npos) << ' ';
		out << '\n';
		return out;
	}
}

class King :public Piece
{
public:
	King(char letter, unsigned int num, Field* pboard, bool white) :Piece(letter, num, pboard, white, true), steps(0)
	{}
	// Sets steps=0 if movement succesfull
	void move_right()
	{
		if (m_board.inside(m_lpos + steps, m_npos + steps) && m_board(m_lpos + steps, m_npos + steps) == nullptr)
		{
			m_board(m_lpos, m_npos) = nullptr;
			m_board(m_lpos + steps, m_npos + steps) = this;
			m_lpos += steps;
			m_npos += steps;
			steps = 0;
		}
	}
	// Sets steps=0 if movement succesfull
	void move_left()
	{
		if (m_board.inside(m_lpos - steps, m_npos + steps) && m_board(m_lpos - steps, m_npos + steps) == nullptr)
		{
			m_board(m_lpos, m_npos) = nullptr;
			m_board(m_lpos - steps, m_npos + steps) = this;
			m_lpos -= steps;
			m_npos += steps;
			steps = 0;
		}
	}
	bool eat()
	{
		int steps;
		bool eaten = false;
		while (eatable_right_up() || eatable_right_down() || eatable_left_up() || eatable_left_down())
		{
			if ((steps = eatable_right_up()) != 0)
			{
				delete m_board(m_lpos + steps, m_npos + steps);
				m_board(m_lpos + steps, m_npos + steps) = nullptr;
				m_board(m_lpos, m_npos) = nullptr;
				m_board(m_lpos + steps + 1, m_npos + steps + 1) = this;
				m_lpos += steps; m_npos += steps;
			}
			else if ((steps = eatable_right_down()) != 0)
			{
				delete m_board(m_lpos + steps, m_npos - steps);
				m_board(m_lpos + steps, m_npos - steps) = nullptr;
				m_board(m_lpos, m_npos) = nullptr;
				m_board(m_lpos + steps + 1, m_npos - steps - 1) = this;
				m_lpos += steps; m_npos -= steps;
			}
			else if ((steps = eatable_left_up()) != 0)
			{
				delete m_board(m_lpos - steps, m_npos + steps);
				m_board(m_lpos - steps, m_npos + steps) = nullptr;
				m_board(m_lpos, m_npos) = nullptr;
				m_board(m_lpos - steps - 1, m_npos + steps + 1) = this;
				m_lpos -= steps; m_npos += steps;
			}
			else if ((steps = eatable_left_down()) != 0)
			{
				delete m_board(m_lpos - steps, m_npos - steps);
				m_board(m_lpos - steps, m_npos - steps) = nullptr;
				m_board(m_lpos, m_npos) = nullptr;
				m_board(m_lpos - steps - 1, m_npos - steps - 1) = this;
				m_lpos -= steps; m_npos -= steps;
			}
			eaten = true;
		}
		return eaten;
	}
	int steps;
private:
	// Returns the number of steps where the piece to be eaten is located (or 0)
	int eatable_right_up() const
	{
		int step = 1;
		while (m_board.inside(m_lpos + step + 1, m_npos + step + 1))
		{
			if ( 
				m_board(m_lpos + step, m_npos + step) != nullptr && m_board(m_lpos + step + 1, m_npos + step + 1) == nullptr && 
				 ( (white() && !m_board(m_lpos + step, m_npos + step)->white()) || (!white() && m_board(m_lpos + step, m_npos + step)->white()) ) 
				)
				return step;
			step++;
		}
		return 0;
	}
	int eatable_right_down() const
	{
		int step = 1;
		while (m_board.inside(m_lpos + step + 1, m_npos - step - 1))
		{
			if (
				m_board(m_lpos + step, m_npos - step) != nullptr && m_board(m_lpos + step + 1, m_npos - step - 1) == nullptr &&
				((white() && !m_board(m_lpos + step, m_npos - step)->white()) || (!white() && m_board(m_lpos + step, m_npos - step)->white()))
				)
				return step;
			step++;
		}
		return 0;
	}
	int eatable_left_up() const
	{
		int step = 1;
		while (m_board.inside(m_lpos - step - 1, m_npos + step + 1))
		{
			if (
				m_board(m_lpos - step, m_npos + step) != nullptr && m_board(m_lpos - step - 1, m_npos + step + 1) == nullptr &&
				((white() && !m_board(m_lpos - step, m_npos + step)->white()) || (!white() && m_board(m_lpos - step, m_npos + step)->white()))
				)
				return step;
			step++;
		}
		return 0;
	}
	int eatable_left_down() const
	{
		int step = 1;
		while (m_board.inside(m_lpos - step - 1, m_npos - step - 1))
		{
			if (
				m_board(m_lpos - step, m_npos - step) != nullptr && m_board(m_lpos - step - 1, m_npos - step - 1) == nullptr &&
				((white() && !m_board(m_lpos - step, m_npos - step)->white()) || (!white() && m_board(m_lpos - step, m_npos - step)->white()))
				)
				return step;
			step++;
		}
		return 0;
	}
};
class WhitePiece :public Piece
{
public:
	WhitePiece(char letter, unsigned int num, Field* pboard) :Piece(letter, num, pboard, true, false)
	{}
	~WhitePiece()
	{}
	void move_right()
	{
		if (m_board.inside(m_lpos + 1, m_npos + 1) && m_board(m_lpos + 1, m_npos + 1) == nullptr)
		{
			m_board(m_lpos, m_npos) = nullptr;
			m_board(m_lpos + 1, m_npos + 1) = this;
			++m_lpos; ++m_npos;
		}
		if (m_npos == 8)
		{
			m_board(m_lpos, m_npos) = new King(m_lpos, m_npos, &m_board, true);
			this->~WhitePiece();
		}
	}
	void move_left()
	{
		if (m_board.inside(m_lpos - 1, m_npos + 1) && m_board(m_lpos - 1, m_npos + 1) == nullptr)
		{
			m_board(m_lpos, m_npos) = nullptr;
			m_board(m_lpos - 1, m_npos + 1) = this;
			--m_lpos; ++m_npos;
		}
		if (m_npos == 8)
		{
			m_board(m_lpos, m_npos) = new King(m_lpos, m_npos, &m_board, true);
			this->~WhitePiece();
		}
	}
	bool eat()
	{
		bool eaten = false;
		while (eatable_left() || eatable_right())
		{
			if (eatable_left())
			{
				delete m_board(m_lpos - 1, m_npos + 1);
				m_board(m_lpos - 1, m_npos + 1) = nullptr;
				m_board(m_lpos, m_npos) = nullptr;
				m_board(m_lpos - 2, m_npos + 2) = this;
				m_lpos -= 2; m_npos += 2;
			}
			else
			{
				delete m_board(m_lpos + 1, m_npos + 1);
				m_board(m_lpos + 1, m_npos + 1) = nullptr;
				m_board(m_lpos, m_npos) = nullptr;
				m_board(m_lpos + 2, m_npos + 2) = this;
				m_lpos += 2; m_npos += 2;
			}
			eaten = true;
		}
		if (m_npos == 8)
		{
			m_board(m_lpos, m_npos) = new King(m_lpos, m_npos, &m_board, true);
			this->~WhitePiece();
		}
		return eaten;
	}
private:
	bool eatable_right()
	{
		return (m_board.inside(m_lpos + 2, m_npos + 2) && m_board(m_lpos + 2, m_npos + 2) == nullptr &&
				m_board(m_lpos + 1, m_npos + 1) != nullptr && !m_board(m_lpos + 1, m_npos + 1)->white());
	}
	bool eatable_left()
	{
		return (m_board.inside(m_lpos - 2, m_npos + 2) && m_board(m_lpos - 2, m_npos + 2) == nullptr &&
				m_board(m_lpos - 1, m_npos + 1) != nullptr && !m_board(m_lpos - 1, m_npos + 1)->white());
	}
};
class BlackPiece :public Piece
{
public:
	BlackPiece(char letter, unsigned int num, Field* pboard) :Piece(letter, num, pboard, false, false)
	{}
	~BlackPiece()
	{}
	void move_right()
	{
		if (m_board.inside(m_lpos + 1, m_npos - 1) && m_board(m_lpos + 1, m_npos - 1) == nullptr)
		{
			m_board(m_lpos, m_npos) = nullptr;
			m_board(m_lpos + 1, m_npos - 1) = this;
			++m_lpos; --m_npos;
		}
		if (m_npos == 1)
		{
			m_board(m_lpos, m_npos) = new King(m_lpos, m_npos, &m_board, false);
			this->~BlackPiece();
		}
	}
	void move_left()
	{
		if (m_board.inside(m_lpos - 1, m_npos - 1) && m_board(m_lpos - 1, m_npos - 1) == nullptr)
		{
			m_board(m_lpos, m_npos) = nullptr;
			m_board(m_lpos - 1, m_npos - 1) = this;
			--m_lpos; ++m_npos;
		}
		if (m_npos == 1)
		{
			m_board(m_lpos, m_npos) = new King(m_lpos, m_npos, &m_board, false);
			this->~BlackPiece();
		}
	}
	bool eat()
	{
		bool eaten = false;
		while (eatable_left() || eatable_right())
		{
			if (eatable_left())
			{
				delete m_board(m_lpos - 1, m_npos - 1);
				m_board(m_lpos - 1, m_npos - 1) = nullptr;
				m_board(m_lpos, m_npos) = nullptr;
				m_board(m_lpos - 2, m_npos - 2) = this;
				m_lpos -= 2; m_npos -= 2;
			}
			else
			{
				delete m_board(m_lpos + 1, m_npos - 1);
				m_board(m_lpos + 1, m_npos - 1) = nullptr;
				m_board(m_lpos, m_npos) = nullptr;
				m_board(m_lpos + 2, m_npos - 2) = this;
				m_lpos += 2; m_npos -= 2;
			}
			eaten = true;
		}
		if (m_npos == 8)
		{
			m_board(m_lpos, m_npos) = new King(m_lpos, m_npos, &m_board, true);
			this->~BlackPiece();
		}
		return eaten;
	}
private:
	bool eatable_right()
	{
		return (m_board.inside(m_lpos + 2, m_npos - 2) && m_board(m_lpos + 2, m_npos - 2) == nullptr &&
				m_board(m_lpos + 1, m_npos - 1) != nullptr && !m_board(m_lpos + 1, m_npos - 1)->white());
	}
	bool eatable_left()
	{
		return (m_board.inside(m_lpos - 2, m_npos - 2) && m_board(m_lpos - 2, m_npos - 2) == nullptr &&
				m_board(m_lpos - 1, m_npos - 1) != nullptr && !m_board(m_lpos - 1, m_npos - 1)->white());
	}
};