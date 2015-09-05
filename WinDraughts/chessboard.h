#pragma once
#include<afxwin.h>
/** GENERIC CHESSBOARD OF VARIABLE SIZE **
* requires use of MFC
* up to 19 x 19
**/

struct Pos
{
	Pos(char letter, unsigned int digit) :letter(letter), digit(digit)
	{ }
	char letter;
	unsigned int digit;
};
template <unsigned int size> class ChessBoard
{
public:
	ChessBoard(CRect* pclient_rect, CDC* pDC) :m_br(0, 0, 0, 0)
	{
		// Border rectangle
		int client_x = (pclient_rect->Size()).cx;
		int client_y = (pclient_rect->Size()).cy;
		unsigned int min_size = (client_x < client_y) ? client_x : client_y;
		int vmargin = pDC->GetTextExtent("White").cy;
		m_br_size = min_size - (8 * vmargin);
		int hmargin = (client_x - m_br_size) / 2;
		m_br.right = m_br.bottom = m_br_size;
		m_br.OffsetRect(hmargin, 6 * vmargin);
		m_square_size = m_br_size / size;
		// Letters, numbers
		CSize letter_size = pDC->GetTextExtent("H");
		CSize num_size = pDC->GetTextExtent("66");
		CPoint letter_corner[size];
		CPoint num_corner[size];
		// Horisontal and vertical rectangles
		for (unsigned int i = 0; i < size; ++i)
		{
			m_vrects[i].left = m_br.left + m_square_size * i;
			m_hrects[i].top = m_br.top + m_square_size * i;
			m_vrects[i].right = m_vrects[i].left + m_square_size + 1;
			m_hrects[i].bottom = m_hrects[i].top + m_square_size + 1;
			m_vrects[i].top = m_br.top;
			m_vrects[i].bottom = m_br.top + m_square_size * size + 1;
			m_hrects[i].left = m_br.left;
			m_hrects[i].right = m_br.left + m_square_size * size + 1;

			num_corner[i] = m_hrects[i].TopLeft();
			num_corner[i].x -= 2 * letter_size.cx;
			num_corner[i].y += m_square_size / 2 - num_size.cy / 2;
			m_numbers[i] = CRect(num_corner[i], num_size);

			letter_corner[i] = m_vrects[i].BottomRight();
			letter_corner[i].x -= (m_square_size / 2 + letter_size.cx / 2);
			m_letters[i] = CRect(letter_corner[i], letter_size);
		}
		// Buttons rectangle
		m_buttons_field = CRect(m_hrects[0].left, 2 * vmargin, m_hrects[0].right, 5 * vmargin);
	}
	virtual ~ChessBoard()
	{ }
	// Rectangle for square at the given position
	CRect SquareAt(char letter, unsigned int num) const
	{
		int vrect_num = letter - 'a';
		int hrect_num = size - num;
		CRect temp;
		temp.IntersectRect(m_vrects + vrect_num, m_hrects + hrect_num);
		return temp;
	}
	// Rectangle for square at the given position
	CRect SquareAt(const Pos& position) const
	{
		return SquareAt(position.letter, position.digit);
	}
	// Center of square at the given position
	CPoint CtrAt(char letter, unsigned int num) const
	{
		return SquareAt(letter, num).CenterPoint();
	}
	// Center of square at the given position
	CPoint CtrAt(const Pos& position) const
	{
		return SquareAt(position.letter, position.digit);
	}
	Pos SquareOfPt(const CPoint& pt) const
	{
		Pos temp(0, 0);
		for (unsigned int num = 1; num <= size; ++num)
			for (char letter = 'a'; letter < 'a' + size; ++letter)
				if (SquareAt(letter, num).PtInRect(pt))
				{
					temp.letter = letter;
					temp.digit = num;
					return temp;
				}
		return temp;
	}
	int SquareSize() const
	{
		return m_square_size;
	}
	virtual void Display(CDC* pDC)
	{
		for (unsigned int i = 0; i < size; ++i)
		{
			pDC->SelectStockObject(HOLLOW_BRUSH);
			pDC->Rectangle(m_vrects + i);
			pDC->SelectStockObject(HOLLOW_BRUSH);
			pDC->Rectangle(m_hrects + i);
			std::string letter, number;
			letter.push_back('a' + i);
			char numchar = (char)(size - i + '0');
			if (numchar > '9')
			{
				number.push_back('1');
				number.push_back(numchar - 10);
			}
			else
				number.push_back(numchar);
			pDC->DrawTextW(letter.c_str(), m_letters + i, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
			pDC->DrawTextW(number.c_str(), m_numbers + i, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
		}
		for (unsigned int row = 1; row <= size; ++row)
			for (char letter = (row % 2) ? 'a' : 'b'; letter < 'a' + size; letter += 2)
			{
				pDC->SelectStockObject(GRAY_BRUSH);
				pDC->Rectangle(SquareAt(letter, row));
			}
	}
protected:
	const CRect& ButtonsField() const
	{
		return m_buttons_field;
	}
private:
	CRect m_br;			// border rectangle
	int m_br_size;		// border rectangle side length
	int m_square_size;
	CRect m_vrects[size];
	CRect m_hrects[size];
	CRect m_letters[size];
	CRect m_numbers[size];
	CRect m_buttons_field;
};